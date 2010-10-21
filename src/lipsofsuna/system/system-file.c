/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
 *
 * Lips of Suna is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Lips of Suna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lips of Suna. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \addtogroup lisys System
 * @{
 * \addtogroup lisysFile File
 * @{
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include "system.h"
#include "system-error.h"
#include "system-file.h"

struct _LISysMmap
{
#ifdef HAVE_SYS_MMAN_H
	int size;
	void* buffer;
#elif defined HAVE_WINDOWS_H
	int size;
	void* buffer;
	HANDLE file;
	HANDLE handle;
#else
#error "Mmap not supported"
#endif
};

/**
 * \brief Creates a read-only memory map of the file.
 *
 * \param path Path.
 * \return New memory map handle or NULL.
 */
LISysMmap*
lisys_mmap_open (const char* path)
{
#ifdef HAVE_SYS_MMAN_H

	int fd;
	struct stat st;
	LISysMmap* self;

	/* Allocate self. */
	self = malloc (sizeof (LISysMmap));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Open the file. */
	fd = open (path, O_RDONLY);
	if (fd == -1)
	{
		lisys_error_set (EIO, "cannot open `%s'", path);
		free (self);
		return NULL;
	}
	if (fstat (fd, &st) == -1)
	{
		lisys_error_set (EIO, "cannot stat `%s'", path);
		close (fd);
		free (self);
		return NULL;
	}
	self->size = st.st_size;

	/* Memory map the file. */
	if (!self->size)
	{
		self->buffer = NULL;
		return self;
	}
	self->buffer = mmap (NULL, self->size, PROT_READ, MAP_SHARED, fd, 0);
	close (fd);
	if (self->buffer == MAP_FAILED)
	{
		lisys_error_set (EIO, "cannot mmap `%s'", path);
		free (self);
		return NULL;
	}

	return self;

#elif defined HAVE_WINDOWS_H

	LISysMmap* self;

	/* Allocate self. */
	self = malloc (sizeof (LISysMmap));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Open the file. */
	self->file = CreateFile (path, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (self->file == NULL)
	{
		lisys_error_set (EIO, "cannot open `%s'", path);
		free (self);
		return NULL;
	}

	/* Create file mapping. */
	self->handle = CreateFileMapping (self->file, NULL, PAGE_READONLY, 0, 0, NULL);
	if (self->handle == NULL)
	{
		lisys_error_set (EIO, "cannot create mapping for `%s'", path);
		CloseHandle (self->file);
		free (self);
		return NULL;
	}
	self->size = GetFileSize (self->handle, NULL);
	if (self->size == INVALID_FILE_SIZE)
	{
		lisys_error_set (EIO, "cannot get size of `%s'", path);
		CloseHandle (self->handle);
		CloseHandle (self->file);
		free (self);
		return NULL;
	}

	/* Memory map the file. */
	if (self->size)
	{
		self->buffer = MapViewOfFile (self->handle, FILE_MAP_READ, 0, 0, self->size);
		if (self->buffer == NULL)
		{
			lisys_error_set (EIO, "cannot map `%s'", path);
			CloseHandle (self->handle);
			CloseHandle (self->file);
			free (self);
			return NULL;
		}
	}
	else
		self->buffer = NULL;

	return self;

#else
#error "Mmap not supported"
	return NULL;
#endif
}

/**
 * \brief Frees a memory map handle.
 *
 * \param self Memory map handle.
 */
void
lisys_mmap_free (LISysMmap* self)
{
#ifdef HAVE_SYS_MMAN_H
	if (self->buffer != NULL)
		munmap ((void*) self->buffer, self->size);
	free (self);
#elif defined HAVE_WINDOWS_H
	if (self->buffer != NULL)
		UnmapViewOfFile ((void*) self->buffer);
	CloseHandle (self->handle);
	CloseHandle (self->file);
#else
#error "Mmap not supported"
	return NULL;
#endif
}

/**
 * \brief Gets the mapped memory area.
 *
 * \param self Memory map.
 * \return Memory pointer, may be NULL if the file size was zero.
 */
void*
lisys_mmap_get_buffer (LISysMmap* self)
{
	return self->buffer;
}

/**
 * \brief Gets the size of the mapped file.
 *
 * \param self Memory map.
 * \return Size in bytes.
 */
int
lisys_mmap_get_size (LISysMmap* self)
{
	return self->size;
}

/**
 * \brief Checks if the file is accessible.
 *
 * \param path Path to the file.
 * \param mode Access mode.
 * \return Nonzero on success.
 */
int
lisys_access (const char* path,
              int         mode)
{
	int m = 0;

	if (mode & LISYS_ACCESS_EXISTS) m |= F_OK;
	if (mode & LISYS_ACCESS_READ) m |= R_OK;
	if (mode & LISYS_ACCESS_WRITE) m |= W_OK;
	if (mode & LISYS_ACCESS_EXEC) m |= X_OK;

	return !access (path, m);
}

/**
 * \brief Reads the target of a symbolic link.
 *
 * \param path Path to the link.
 * \return Link target or NULL.
 */
char*
lisys_readlink (const char* path)
{
#ifdef HAVE_READLINK

	int len;
	char* tmp;
	char* name;

	len = 128;
	name = NULL;
	while (1)
	{
		tmp = realloc (name, len);
		if (tmp == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			free (name);
			return NULL;
		}
		name = tmp;
		name[len - 1] = '\0';
		if (readlink (path, name, len) <= 0)
		{
			lisys_error_set (errno, "cannot read link `%s'", path);
			free (name);
			return NULL;
		}
		if (strlen (tmp) < len)
			break;
		len <<= 1;
	}

	return name;

#else
	lisys_error_set (ENOTSUP, NULL);
	return NULL;
#endif
}

/**
 * \brief Stats an open file.
 *
 * \param path Path to stat.
 * \param result Return value for the stat data.
 * \return Nonzero on success.
 */
int
lisys_stat (const char* path,
            LISysStat*  result)
{
	struct stat st;

	/* Stat the file. */
	if (stat (path, &st) != 0)
	{
		lisys_error_set (EIO, "cannot stat `%s'", path);
		return 0;
	}

	/* Convert results. */
	if (S_ISREG (st.st_mode)) result->type = LISYS_STAT_FILE;
	else if (S_ISDIR (st.st_mode)) result->type = LISYS_STAT_DIRECTORY;
	else if (S_ISCHR (st.st_mode)) result->type = LISYS_STAT_CHAR;
	else if (S_ISBLK (st.st_mode)) result->type = LISYS_STAT_BLOCK;
#ifdef HAVE_LSTAT
	else if (S_ISLNK (st.st_mode)) result->type = LISYS_STAT_LINK;
#endif
	else { lisys_assert (0); }
	result->uid = st.st_uid;
	result->gid = st.st_gid;
	result->mode = st.st_mode & 0777;
	result->size = st.st_size;
	result->mtime = st.st_mtime;

	return 1;
}

/**
 * \brief Stats the specified path.
 *
 * \param path Path to stat.
 * \param result Return value for the stat data.
 * \return Nonzero on success.
 */
int
lisys_lstat (const char* path,
             LISysStat*  result)
{
#ifdef HAVE_LSTAT

	struct stat st;

	/* Stat the file. */
	if (lstat (path, &st) != 0)
	{
		lisys_error_set (EIO, "cannot stat `%s'", path);
		return 0;
	}

	/* Convert results. */
	if (S_ISREG (st.st_mode)) result->type = LISYS_STAT_FILE;
	else if (S_ISDIR (st.st_mode)) result->type = LISYS_STAT_DIRECTORY;
	else if (S_ISCHR (st.st_mode)) result->type = LISYS_STAT_CHAR;
	else if (S_ISBLK (st.st_mode)) result->type = LISYS_STAT_BLOCK;
#ifdef HAVE_LSTAT
	else if (S_ISLNK (st.st_mode)) result->type = LISYS_STAT_LINK;
#endif
	else { lisys_assert (0); }
	result->uid = st.st_uid;
	result->gid = st.st_gid;
	result->mode = st.st_mode & 0777;
	result->size = st.st_size;
	result->mtime = st.st_mtime;
	result->rdev = st.st_rdev;

	return 1;

#else

	return lisys_stat (path, result);

#endif
}

/** @} */
/** @} */

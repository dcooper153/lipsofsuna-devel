/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup lirel Reload
 * @{
 * \addtogroup lirelReload Reload
 * @{
 */

#include <sys/stat.h>
#include <model/lips-model.h>
#include <string/lips-string.h>
#include "reload.h"

static void
private_async_reload (lithrAsyncCall* call,
                      void*           data);

static int
private_convert_models (lithrAsyncCall* call,
                        lirelReload*    self,
                        const char*     srcdir,
                        const char*     dstdir);

static int
private_convert_textures (lithrAsyncCall* call,
                          lirelReload*    self,
                          const char*     srcdir,
                          const char*     dstdir);

static int
private_filter_blend_modified (const char* srcdir,
                               const char* name,
                               void*       dstdir);

static int
private_filter_img_modified (const char* srcdir,
                             const char* name,
                             void*       dstdir);

static int
private_filter_xcf_modified (const char* srcdir,
                             const char* name,
                             void*       dstdir);

/*****************************************************************************/

/**
 * \brief Creates a new data reloader for the engine.
 *
 * The reload object provides automatic data file reloading services. When
 * modifications to source data files are detected in the update call, a worker
 * thread is created that converts the source file to the native format
 * supported by the game. When the worker thread has finished the conversion or
 * if the user has manually edited the native data file, a user provided
 * callback is called so that, for example, the engine can reload the texture
 * or model in question.
 *
 * \param paths Path information.
 * \return New reloader or NULL.
 */
lirelReload*
lirel_reload_new (lipthPaths* paths)
{
	lirelReload* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirelReload));
	if (self == NULL)
		return NULL;
	self->paths = paths;

	return self;
}

void
lirel_reload_free (lirelReload* self)
{
	lirel_reload_cancel (self);
	lisys_free (self);
}

/**
 * \brief Cancels any active reloading process.
 *
 * \param self Reload.
 */
void
lirel_reload_cancel (lirelReload* self)
{
	if (self->worker == NULL)
		return;
	lithr_async_call_stop (self->worker);
	lithr_async_call_join (self->worker);
	lithr_async_call_free (self->worker);
	self->worker = NULL;
}

int
lirel_reload_main (lirelReload* self)
{
	if (!lirel_reload_run (self))
		return 0;
	while (self->worker != NULL)
	{
		lirel_reload_update (self);
		lisys_usleep (1000);
	}

	return 1;
}

/**
 * \brief Reloads all client data files.
 *
 * \param self Reload.
 * \return Nonzero on success.
 */
int
lirel_reload_run (lirelReload* self)
{
	/* Make sure not already running. */
	if (self->worker != NULL)
	{
		lisys_error_set (EBUSY, "busy reloading");
		return 0;
	}

	/* Create worker thread. */
	self->worker = lithr_async_call_new (private_async_reload, NULL, self);
	if (self->worker == NULL)
		return 0;

	return 1;
}

/**
 * \brief Monitors data files for changes.
 *
 * \param self Reload.
 * \return Nonzero on success.
 */
int
lirel_reload_update (lirelReload* self)
{
	char* name;

	/* Terminate finished jobs. */
	if (self->worker != NULL && lithr_async_call_get_done (self->worker))
	{
		lithr_async_call_free (self->worker);
		self->worker = NULL;
	}

	/* Wait for monitor events. */
	if (self->notify == NULL)
		return 1;
	if (!lisys_notify_poll (self->notify))
		return 1;
	if (!(self->notify->event.flags & LISYS_NOTIFY_CLOSEW))
		return 1;

	/* Reload changed models. */
	if (lisys_path_check_ext (self->notify->event.name, "lmdl"))
	{
		if (self->reload_model_call != NULL)
		{
			name = lisys_path_format (LISYS_PATH_BASENAME,
				self->notify->event.name, LISYS_PATH_STRIPEXTS, NULL);
			if (name != NULL)
			{
				self->reload_model_call (self->reload_model_data, name);
				lisys_free (name);
			}
		}
	}

	/* Reload changed DDS textures. */
	if (lisys_path_check_ext (self->notify->event.name, "dds"))
	{
		if (self->reload_image_call != NULL)
		{
			name = lisys_path_format (LISYS_PATH_BASENAME,
				self->notify->event.name, LISYS_PATH_STRIPEXTS, NULL);
			if (name != NULL)
			{
				self->reload_image_call (self->reload_image_data, name);
				lisys_free (name);
			}
		}
	}

	/* Initiate conversion if an original has changed. */
	if (lisys_path_check_ext (self->notify->event.name, "blend") ||
	    lisys_path_check_ext (self->notify->event.name, "blend.gz") ||
	    lisys_path_check_ext (self->notify->event.name, "jpg") ||
	    lisys_path_check_ext (self->notify->event.name, "png") ||
	    lisys_path_check_ext (self->notify->event.name, "xcf") ||
	    lisys_path_check_ext (self->notify->event.name, "xcf.bz2") ||
	    lisys_path_check_ext (self->notify->event.name, "xcf.gz") ||
	    lisys_path_check_ext (self->notify->event.name, "xcfbz2") ||
	    lisys_path_check_ext (self->notify->event.name, "xcfgz"))
	{
		printf ("Reloading originals...\n");
		self->queued = 1;
	}
	if (self->queued && self->worker == NULL)
	{
		if (lirel_reload_run (self))
			self->queued = 0;
	}

	return 1;
}

int
lirel_reload_get_done (const lirelReload* self)
{
	return self->worker == NULL;
}

int
lirel_reload_get_enabled (const lirelReload* self)
{
	return (self->notify != NULL);
}

int
lirel_reload_set_enabled (lirelReload* self,
                          int          value)
{
	char* srcdir;
	char* dstdir;

	if ((value != 0) == (self->notify != NULL))
		return 1;
	if (value)
	{
		self->notify = lisys_notify_new ();
		if (self->notify == NULL)
			return 0;
		srcdir = lisys_path_concat (self->paths->module_data, "import", NULL);
		dstdir = lisys_path_concat (self->paths->module_data, "graphics", NULL);
		if (srcdir == NULL || dstdir == NULL)
		{
			lisys_notify_free (self->notify);
			lisys_free (srcdir);
			lisys_free (dstdir);
			self->notify = NULL;
			return 0;
		}
		if (!lisys_notify_add (self->notify, srcdir, LISYS_NOTIFY_CLOSEW) ||
		    !lisys_notify_add (self->notify, dstdir, LISYS_NOTIFY_CLOSEW))
		{
			lisys_notify_free (self->notify);
			self->notify = NULL;
			lisys_free (srcdir);
			lisys_free (dstdir);
		}
		lisys_free (srcdir);
		lisys_free (dstdir);
	}
	else
	{
		lisys_notify_free (self->notify);
		self->notify = NULL;
	}

	return 1;
}

void
lirel_reload_set_image_callback (lirelReload* self,
                                 void       (*call)(),
                                 void*        data)
{
	self->reload_image_call = call;
	self->reload_image_data = data;
}

void
lirel_reload_set_model_callback (lirelReload* self,
                                 void       (*call)(),
                                 void*        data)
{
	self->reload_model_call = call;
	self->reload_model_data = data;
}

float
lirel_reload_get_progress (const lirelReload* self)
{
	if (self->worker == NULL)
		return 1.0f;
	return lithr_async_call_get_progress (self->worker);
}

/*****************************************************************************/

static void
private_async_reload (lithrAsyncCall* call,
                      void*           data)
{
	char* srcdir;
	char* dstdir;
	lirelReload* self = data;

	/* Convert textures. */
	srcdir = lisys_path_concat (self->paths->module_data, "import", NULL);
	dstdir = lisys_path_concat (self->paths->module_data, "graphics", NULL);
	if (srcdir == NULL || dstdir == NULL)
		goto error;
	if (!private_convert_textures (call, self, srcdir, dstdir))
		goto error;
	if (lithr_async_call_get_stop (call))
		goto stop;

	/* Convert models. */
	if (!private_convert_models (call, self, srcdir, dstdir))
		goto error;
	if (lithr_async_call_get_stop (call))
		goto stop;

	/* The rest is done in the free callback. */
	lithr_async_call_set_result (call, 1);

stop:
error:
	lisys_free (srcdir);
	lisys_free (dstdir);
}

static int
private_convert_models (lithrAsyncCall* call,
                        lirelReload*    self,
                        const char*     srcdir,
                        const char*     dstdir)
{
	int i;
	int count;
	char* src = NULL;
	char* dst = NULL;
	lisysDir* directory = NULL;

	/* Find all modified model sources. */
	directory = lisys_dir_open (srcdir);
	if (directory == NULL)
		return 0;
	lisys_dir_set_filter (directory, private_filter_blend_modified, (void*) dstdir);
	lisys_dir_set_sorter (directory, LISYS_DIR_SORTER_ALPHA);
	if (!lisys_dir_scan (directory))
	{
		lisys_dir_free (directory);
		return 0;
	}
	count = lisys_dir_get_count (directory);

	/* Convert modified models. */
	for (i = 0 ; i < count ; i++)
	{
		lithr_async_call_set_progress (call, (float) i / count);
		if (lithr_async_call_get_stop (call))
			break;
		src = lisys_path_format (srcdir,
			LISYS_PATH_SEPARATOR, lisys_dir_get_name (directory, i), NULL);
		dst = lisys_path_format (dstdir,
			LISYS_PATH_SEPARATOR, lisys_dir_get_name (directory, i),
			LISYS_PATH_STRIPEXTS, ".lmdl", NULL);
		if (src == NULL || dst == NULL)
		{
			lisys_dir_free (directory);
			lisys_free (src);
			lisys_free (dst);
		}
		if (!lirel_reload_blender (self, src, dst))
			lisys_error_report ();
		lisys_free (src);
		lisys_free (dst);
	}
	lisys_dir_free (directory);

	return 1;
}

static int
private_convert_textures (lithrAsyncCall* call,
                          lirelReload*    self,
                          const char*     srcdir,
                          const char*     dstdir)
{
	int i;
	int j;
	int count;
	char* src;
	char* dst;
	lisysDir* directory = NULL;
	const struct
	{
		int (*filter)(const char*, const char*, void*);
		int (*convert)(lirelReload*, const char*, const char*);
	}
	converters[] =
	{
		{ private_filter_xcf_modified, lirel_reload_gimp },
		{ private_filter_img_modified, lirel_reload_image }
	};

	/* Convert all modified texture sources. */
	for (j = 0 ; j < sizeof (converters) / sizeof (*converters) ; j++)
	{
		src = NULL;
		dst = NULL;
		directory = lisys_dir_open (srcdir);
		if (directory == NULL)
			return 0;
		lisys_dir_set_filter (directory, converters[j].filter, (void*) dstdir);
		lisys_dir_set_sorter (directory, LISYS_DIR_SORTER_ALPHA);
		if (!lisys_dir_scan (directory))
		{
			lisys_dir_free (directory);
			return 0;
		}
		count = lisys_dir_get_count (directory);
		for (i = 0 ; i < count ; i++)
		{
			lithr_async_call_set_progress (call, (float) i / count);
			if (lithr_async_call_get_stop (call))
				break;
			src = lisys_path_format (srcdir,
				LISYS_PATH_SEPARATOR, lisys_dir_get_name (directory, i), NULL);
			dst = lisys_path_format (dstdir,
				LISYS_PATH_SEPARATOR, lisys_dir_get_name (directory, i),
				LISYS_PATH_STRIPEXTS, ".dds", NULL);
			if (src == NULL || dst == NULL)
			{
				lisys_dir_free (directory);
				lisys_free (src);
				lisys_free (dst);
			}
			if (!converters[j].convert (self, src, dst))
				lisys_error_report ();
			lisys_free (src);
			lisys_free (dst);
		}
		lisys_dir_free (directory);
	}

	return 1;
}

static int
private_filter_blend_modified (const char* srcdir,
                               const char* name,
                               void*       dstdir)
{
	int ret;
	char* src;
	char* dst;
	struct stat src_st;
	struct stat dst_st;

	/* Check for extension. */
	if (!lisys_path_check_ext (name, "blend") &&
	    !lisys_path_check_ext (name, "blend.gz"))
		return 0;

	/* Construct file names. */
	src = lisys_path_concat (srcdir, name, NULL);
	if (src == NULL)
		return 1;
	dst = lisys_path_format (dstdir, LISYS_PATH_SEPARATOR, name, LISYS_PATH_STRIPEXTS, ".lmdl", NULL);
	if (dst == NULL)
	{
		lisys_free (src);
		return 1;
	}

	/* Check for modifications. */
	ret = stat (src, &src_st);
	ret |= stat (dst, &dst_st);
	lisys_free (src);
	lisys_free (dst);
	if (ret != 0)
		return 1;
	if (src_st.st_mtime < dst_st.st_mtime)
		return 0;

	return 1;
}

static int
private_filter_img_modified (const char* srcdir,
                             const char* name,
                             void*       dstdir)
{
	int ret;
	char* src;
	char* dst;
	struct stat src_st;
	struct stat dst_st;

	/* Check for extension. */
	if (!lisys_path_check_ext (name, "jpg") &&
	    !lisys_path_check_ext (name, "png"))
		return 0;

	/* Construct file names. */
	src = lisys_path_concat (srcdir, name, NULL);
	if (src == NULL)
		return 1;
	dst = lisys_path_format (dstdir, LISYS_PATH_SEPARATOR, name, LISYS_PATH_STRIPEXTS, ".dds", NULL);
	if (dst == NULL)
	{
		lisys_free (src);
		return 1;
	}

	/* Check for modifications. */
	ret = stat (src, &src_st);
	ret |= stat (dst, &dst_st);
	lisys_free (src);
	lisys_free (dst);
	if (ret != 0)
		return 1;
	if (src_st.st_mtime < dst_st.st_mtime)
		return 0;

	return 1;
}

static int
private_filter_xcf_modified (const char* srcdir,
                             const char* name,
                             void*       dstdir)
{
	int ret;
	char* src;
	char* dst;
	struct stat src_st;
	struct stat dst_st;

	/* Check for extension. */
	if (!lisys_path_check_ext (name, "xcf") &&
	    !lisys_path_check_ext (name, "xcf.gz"))
		return 0;

	/* Construct file names. */
	src = lisys_path_concat (srcdir, name, NULL);
	if (src == NULL)
		return 1;
	dst = lisys_path_format (dstdir, LISYS_PATH_SEPARATOR, name, LISYS_PATH_STRIPEXTS, ".dds", NULL);
	if (dst == NULL)
	{
		lisys_free (src);
		return 1;
	}

	/* Check for modifications. */
	ret = stat (src, &src_st);
	ret |= stat (dst, &dst_st);
	lisys_free (src);
	lisys_free (dst);
	if (ret != 0)
		return 1;
	if (src_st.st_mtime < dst_st.st_mtime)
		return 0;

	return 1;
}

/** @} */
/** @} */
/** @} */

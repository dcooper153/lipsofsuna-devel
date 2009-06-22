/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup lisysModule Module
 * @{
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include "system-error.h"
#include "system-module.h"

struct _lisysModule
{
#ifdef HAVE_DLFCN_H
	void* handle;
#elif defined HAVE_WINDOWS_H
	HMODULE handle;
#else
#error "No module loading support"
#endif
};

/**
 * \brief Dynamically loads a module.
 *
 * \param path Path to the file.
 * \param flags Flags.
 * \return New module or NULL.
 */
lisysModule*
lisys_module_new (const char* path,
                  int         flags)
{
#ifdef HAVE_DLFCN_H

	int dlflags;
	lisysModule* self;

	/* Allocate self. */
	self = calloc (1, sizeof (lisysModule));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Get flags. */
	dlflags = RTLD_LAZY;
	if (flags & LISYS_MODULE_FLAG_GLOBAL)
		dlflags |= RTLD_GLOBAL;
	else
		dlflags |= RTLD_LOCAL;

	/* Open library. */
	self->handle = dlopen (path, dlflags);
	if (self->handle == NULL)
	{
		lisys_error_set (EIO, "%s", dlerror ());
		free (self);
		return NULL;
	}

	return self;

#elif defined HAVE_WINDOWS_H

	lisysModule* self;

	/* Allocate self. */
	self = calloc (1, sizeof (lisysModule));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Open library. */
	self->handle = LoadLibraryA (path);
	if (self->handle == 0)
	{
		lisys_error_set (EIO, NULL);
		free (self);
		return NULL;
	}

	return self;

#else
#error "No module loading support"
	return NULL;
#endif
}


/**
 * \brief Closes the module.
 *
 * \param self Module.
 */
void
lisys_module_free (lisysModule* self)
{
#ifdef HAVE_DLFCN_H
	dlclose (self->handle);
	free (self);
#elif defined HAVE_WINDOWS_H
	FreeLibrary (self->handle);
	free (self);
#else
#error "No module loading support"
	free (self);
#endif
}

/**
 * \brief Gets a symbol from the module.
 *
 * \param self Module.
 * \param name Symbol name.
 * \return Symbol or NULL.
 */
void*
lisys_module_symbol (lisysModule* self,
                     const char*  name)
{
#ifdef HAVE_DLFCN_H
	return dlsym (self->handle, name);
#elif defined HAVE_WINDOWS_H
	return GetProcAddress (self->handle, name);
#else
#error "No module loading support"
	return NULL;
#endif
}

/**
 * \brief Gets a global symbol from an already loaded library.
 *
 * \param module Module name without any lib prefix or the extension.
 * \param name Function name.
 * \return Symbol or NULL.
 */
void*
lisys_module_global_symbol (const char* module,
                            const char* name)
{
#ifdef HAVE_DLFCN_H
	void* handle;
	handle = dlopen (NULL, RTLD_NOW);
	return dlsym (handle, name);
#elif defined HAVE_WINDOWS_H
	HANDLE handle;
	handle = GetModuleHandle (name);
	if (handle == NULL)
		return NULL;
	return GetProcAddress (handle, name);
#else
#error "No module loading support"
	return NULL;
#endif
}

/** @} */
/** @} */

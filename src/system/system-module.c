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

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "system-error.h"
#include "system-module.h"

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
}

/**
 * \brief Closes the module.
 *
 * \param self Module.
 */
void
lisys_module_free (lisysModule* self)
{
	dlclose (self->handle);
	free (self);
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
	return dlsym (self->handle, name);
}

/** @} */
/** @} */

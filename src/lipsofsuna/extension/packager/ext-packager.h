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
 * \addtogroup liext Extension
 * @{
 * \addtogroup LIExtPackager Packager
 * @{
 */

#ifndef __EXT_PACKAGER_H__
#define __EXT_PACKAGER_H__

#include "ext-module.h"
#include "ext-resources.h"

struct _LIExtPackager
{
	int verbose;
	LICliClient* client;
	LIMaiProgram* program;
	LIThrAsyncCall* worker;
};

struct _LIExtPackagerData
{
	char* target;
	char* directory;
	LIArcTar* tar;
	LIArcWriter* writer;
	LICliClient* client;
	LIExtPackager* packager;
	LIExtResources* resources;
};

LIExtPackager*
liext_packager_new (LIMaiProgram* program);

void
liext_packager_free (LIExtPackager* self);

void
liext_packager_cancel (LIExtPackager* self);

int
liext_packager_save (LIExtPackager* self,
                     const char*    name,
                     const char*    dir);

int
liext_packager_get_verbose (LIExtPackager* self);

void
liext_packager_set_verbose (LIExtPackager* self,
                            int            value);

#endif

/** @} */
/** @} */

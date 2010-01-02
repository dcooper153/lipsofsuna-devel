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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliPackager Packager
 * @{
 */

#ifndef __EXT_PACKAGER_H__
#define __EXT_PACKAGER_H__

#include <archive/lips-archive.h>
#include <client/lips-client.h>
#include <config/lips-config.h>
#include <thread/lips-thread.h>
#include "ext-resources.h"

typedef struct _liextPackagerFile liextPackagerFile;
struct _liextPackagerFile
{
	char* src;
	char* dst;
};

typedef struct _liextPackager liextPackager;
struct _liextPackager
{
	int verbose;
	licliClient* client;
	lithrAsyncCall* worker;
	liwdgWidget* progress;
};

typedef struct _liextPackagerData liextPackagerData;
struct _liextPackagerData
{
	char* target;
	char* directory;
	liarcTar* tar;
	liarcWriter* writer;
	licliClient* client;
	liextPackager* packager;
	liextResources* resources;
	struct
	{
		int count;
		liextPackagerFile* array;
	} files;
};

liextPackager*
liext_packager_new (licliClient* client);

void
liext_packager_free (liextPackager* self);

void
liext_packager_cancel (liextPackager* self);

int
liext_packager_save (liextPackager* self,
                     const char*    name,
                     const char*    dir);

int
liext_packager_get_verbose (liextPackager* self);

void
liext_packager_set_verbose (liextPackager* self,
                            int            value);

#endif

/** @} */
/** @} */
/** @} */

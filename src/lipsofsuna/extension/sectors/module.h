/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#ifndef __EXT_SECTORS_MODULE_H__
#define __EXT_SECTORS_MODULE_H__

#include "lipsofsuna/extension.h"

#define LIEXT_SCRIPT_SECTORS "Sectors"

typedef struct _LIExtSectorsModule LIExtSectorsModule;
struct _LIExtSectorsModule
{
	LIAlgSectors* sectors;
	LIMaiProgram* program;
};

LIAPICALL (LIExtSectorsModule*, liext_sectors_new, (
	LIMaiProgram* program));

LIAPICALL (void, liext_sectors_free, (
	LIExtSectorsModule* self));

LIAPICALL (void, liext_sectors_update, (
	LIExtSectorsModule* self,
	float               secs));

/*****************************************************************************/

void liext_script_sectors (
	LIScrScript* self);

#endif

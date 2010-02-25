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
 * \addtogroup liextcliGenerator Generator
 * @{
 */

#ifndef __EXT_BLOCK_H__
#define __EXT_BLOCK_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/voxel.h>
#include "ext-preview.h"

typedef struct _LIExtBlock LIExtBlock;
struct _LIExtBlock
{
	LICliClient* client;
	LIExtPreview* preview;
	LIMdlModel* mmodel;
	LIRenModel* rmodel;
	LIRenGroup* group;
};

LIExtBlock*
liext_block_new (LIExtPreview* preview);

void
liext_block_free (LIExtBlock* self);

int
liext_block_build (LIExtBlock*     self,
                   LIVoxBlockAddr* addr);

void
liext_block_clear (LIExtBlock* self);

#endif

/** @} */
/** @} */
/** @} */

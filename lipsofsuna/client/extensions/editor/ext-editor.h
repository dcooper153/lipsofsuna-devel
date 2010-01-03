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
 * \addtogroup liextcliEditor Editor
 * @{
 */

#ifndef __EXT_EDITOR_H__
#define __EXT_EDITOR_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/client.h>
#include <lipsofsuna/math.h>

enum
{
	LIEXT_DRAG_NONE,
	LIEXT_DRAG_ROTATE,
	LIEXT_DRAG_ROTATEX,
	LIEXT_DRAG_ROTATEY,
	LIEXT_DRAG_ROTATEZ,
	LIEXT_DRAG_TRANSLATE,
	LIEXT_DRAG_TRANSLATEX,
	LIEXT_DRAG_TRANSLATEY,
	LIEXT_DRAG_TRANSLATEZ,
};

typedef struct _LIExtEditor LIExtEditor;
struct _LIExtEditor
{
	LICalHandle calls[1];
	LICliClient* client;
	struct
	{
		int mode;
		LIMatVector start;
	} drag;
};

LIExtEditor*
liext_editor_new (LICliClient* client);

void
liext_editor_free (LIExtEditor* self);

void
liext_editor_begin_rotate (LIExtEditor* self,
                           int          x,
                           int          y);

void
liext_editor_begin_translate (LIExtEditor* self,
                              int          x,
                              int          y);

int
liext_editor_create (LIExtEditor*          self,
                     uint32_t              model,
                     const LIMatTransform* transform);

int
liext_editor_destroy (LIExtEditor* self);

int
liext_editor_duplicate (LIExtEditor* self);

void
liext_editor_rotate (LIExtEditor*           self,
                     const LIMatQuaternion* rotation);

void
liext_editor_snap (LIExtEditor* self,
                   float        grid,
                   float        rad);

void
liext_editor_translate (LIExtEditor*       self,
                        const LIMatVector* translation);

void
liext_editor_transform_apply (LIExtEditor* self);

void
liext_editor_transform_cancel (LIExtEditor* self);

void
liext_editor_get_center (LIExtEditor* self,
                         LIMatVector* value);

#endif

/** @} */
/** @} */
/** @} */

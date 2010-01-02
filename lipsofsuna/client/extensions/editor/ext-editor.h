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

#include <algorithm/lips-algorithm.h>
#include <client/lips-client.h>
#include <math/lips-math.h>

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

typedef struct _liextEditor liextEditor;
struct _liextEditor
{
	licalHandle calls[1];
	licliClient* client;
	struct
	{
		int mode;
		limatVector start;
	} drag;
};

liextEditor*
liext_editor_new (licliClient* client);

void
liext_editor_free (liextEditor* self);

void
liext_editor_begin_rotate (liextEditor* self,
                           int          x,
                           int          y);

void
liext_editor_begin_translate (liextEditor* self,
                              int          x,
                              int          y);

int
liext_editor_create (liextEditor*          self,
                     uint32_t              model,
                     const limatTransform* transform);

int
liext_editor_destroy (liextEditor* self);

int
liext_editor_duplicate (liextEditor* self);

void
liext_editor_rotate (liextEditor*           self,
                     const limatQuaternion* rotation);

void
liext_editor_snap (liextEditor* self,
                   float        grid,
                   float        rad);

void
liext_editor_translate (liextEditor*       self,
                        const limatVector* translation);

void
liext_editor_transform_apply (liextEditor* self);

void
liext_editor_transform_cancel (liextEditor* self);

void
liext_editor_get_center (liextEditor* self,
                         limatVector* value);

#endif

/** @} */
/** @} */
/** @} */

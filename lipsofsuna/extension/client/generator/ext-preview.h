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

#ifndef __EXT_PREVIEW_H__
#define __EXT_PREVIEW_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/generator.h>
#include <lipsofsuna/engine.h>
#include <lipsofsuna/widget.h>

#define LIEXT_PREVIEW(o) ((LIExtPreview*)(o))

enum
{
	LIEXT_PREVIEW_COPY_VOXEL,
	LIEXT_PREVIEW_PASTE_VOXEL,
	LIEXT_PREVIEW_ERASE_VOXEL,
	LIEXT_PREVIEW_INSERT_VOXEL,
	LIEXT_PREVIEW_REPLACE_VOXEL,
	LIEXT_PREVIEW_ROTATE_VOXEL,
};

typedef struct _LIExtPreview LIExtPreview;
struct _LIExtPreview
{
	LIWdgRender base;
	LIAlgCamera* camera;
	LIAlgSectors* sectors;
	LIAlgPtrdic* objects;
	LICalCallbacks* callbacks;
	LICalHandle calls[2];
	LICliClient* client;
	LIGenGenerator* generator;
	LIMatVector drag;
	LIMatTransform transform;
	LIRenLight* light0;
	LIRenLight* light1;
	LIRenGroup* group;
	LIRenRender* render;
	LIRenScene* scene;
	int mode;
};

extern const LIWdgClass liext_widget_preview;

LIWdgWidget*
liext_preview_new (LIWdgManager* manager,
                   LICliClient*  client);

void
liext_preview_build (LIExtPreview* self);

int
liext_preview_build_brush (LIExtPreview* self,
                           LIGenBrush*   brush,
                           int           object);

int
liext_preview_build_rule (LIExtPreview* self,
                          LIGenBrush*   brush,
                          LIGenRule*    rule,
                          int           stroke);

int
liext_preview_build_tile (LIExtPreview* self,
                          int           material);

int
liext_preview_clear (LIExtPreview* self);

void
liext_preview_copy_voxels (LIExtPreview* self,
                           int           startx,
                           int           starty,
                           int           startz,
                           int           sizex,
                           int           sizey,
                           int           sizez,
                           LIVoxVoxel*   result);

int
liext_preview_insert_object (LIExtPreview*         self,
                             const LIMatTransform* transform,
                             const char*           model);

int
liext_preview_insert_stroke (LIExtPreview* self,
                             int           x,
                             int           y,
                             int           z,
                             int           brush);

void
liext_preview_paint_terrain (LIExtPreview* self,
                             LIMatVector*  point,
                             int           mode,
                             int           material,
                             int           axis);

int
liext_preview_replace_materials (LIExtPreview* self,
                                 LIArcReader*  reader);

void
liext_preview_setup_camera (LIExtPreview* self,
                            LIMatVector*  eye,
                            LIMatVector*  ctr,
                            LIMatVector*  up,
                            int           driver);

void
liext_preview_get_bounds (LIExtPreview* self,
                          LIMatAabb*    aabb);

void
liext_preview_get_camera_transform (LIExtPreview*   self,
                                    LIMatTransform* value);

void
liext_preview_get_transform (LIExtPreview*   self,
                             LIMatTransform* value);

#endif

/** @} */
/** @} */
/** @} */

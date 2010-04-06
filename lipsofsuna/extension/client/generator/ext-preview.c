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

#include "ext-module.h"
#include "ext-preview.h"
#include "ext-block.h"

#define LIEXT_PREVIEW_CENTER 1024
#define LIEXT_PREVIEW_PAN 0.3f

enum
{
	LIEXT_PREVIEW_MODE_CAMERA,
	LIEXT_PREVIEW_MODE_ROTATE,
	LIEXT_PREVIEW_MODE_ROTATEX,
	LIEXT_PREVIEW_MODE_ROTATEY,
	LIEXT_PREVIEW_MODE_ROTATEZ,
	LIEXT_PREVIEW_MODE_TRANSLATE,
	LIEXT_PREVIEW_MODE_TRANSLATEX,
	LIEXT_PREVIEW_MODE_TRANSLATEY,
	LIEXT_PREVIEW_MODE_TRANSLATEZ
};

static int
private_init (LIExtPreview* self,
              LIWdgManager* manager);

static void
private_free (LIExtPreview* self);

static int
private_event (LIExtPreview* self,
               LIWdgEvent*   event);

static int
private_block_free (LIExtPreview*     self,
                    LIVoxUpdateEvent* event);

static int
private_block_load (LIExtPreview*     self,
                    LIVoxUpdateEvent* event);

static void
private_motion (LIExtPreview* self,
                LIWdgEvent*   event);

static void
private_render_preview (LIWdgWidget*  widget,
                        LIExtPreview* self);

/****************************************************************************/

const LIWdgClass*
liext_widget_preview ()
{
	static LIWdgClass clss =
	{
		NULL, "GeneratorPreview", sizeof (LIExtPreview),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event
	};
	clss.base = liwdg_widget_render;
	return &clss;
}

LIWdgWidget*
liext_preview_new (LIWdgManager* manager,
                   LICliClient*  client)
{
	const float diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float equation[3] = { 1.0f, 0.0f, 0.001f };
	LIExtPreview* data;
	LIMatTransform transform;
	LIWdgWidget* self;

	/* Allocate self. */
	self = liwdg_widget_new (manager, liext_widget_preview ());
	if (self == NULL)
		return NULL;
	data = LIEXT_PREVIEW (self);
	data->client = client;
	data->render = client->render;

	/* Allocate scene. */
	data->scene = liren_scene_new (data->render);
	if (data->scene == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	LIWDG_RENDER (self)->scene = data->scene;

	/* Allocate group. */
	data->group = liren_group_new (data->scene);
	if (data->group == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	liren_group_set_realized (data->group, 1);

	/* Allocate objects. */
	data->objects = lialg_ptrdic_new ();
	if (data->objects == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}

	/* Allocate terrain. */
	data->blocks = lialg_memdic_new ();
	if (data->blocks == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}

	/* Allocate generator. */
	data->callbacks = lical_callbacks_new ();
	data->sectors = lialg_sectors_new (client->sectors->count, client->sectors->width);
	if (data->callbacks == NULL || data->sectors == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	data->generator = ligen_generator_new (client->paths, data->callbacks, data->sectors);
	if (data->generator == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	ligen_generator_set_fill (data->generator, -1);

	/* Register voxel callbacks. */
	if (!lical_callbacks_insert (data->generator->voxels->callbacks, data->generator->voxels,
		"block-free", 1, private_block_free, self, data->calls + 0) ||
	    !lical_callbacks_insert (data->generator->voxels->callbacks, data->generator->voxels,
		"block-load", 1, private_block_load, self, data->calls + 1))
	{
		liwdg_widget_free (self);
		return NULL;
	}

	/* Create camera. */
	data->camera = lialg_camera_new (client->engine);
	if (data->camera == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	transform.position = limat_vector_init (
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f)/* - 15*/,
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f) + 55,
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f)/* - 30.0f*/);
	transform.rotation = limat_quaternion_look (
		limat_vector_init (0.0f, -1.0f, 0.0f),
		limat_vector_init (0.0f, 0.0f, 1.0f));
	lialg_camera_set_driver (data->camera, LIALG_CAMERA_MANUAL);
	lialg_camera_set_transform (data->camera, &transform);
	lialg_camera_warp (data->camera);

	/* Create lights. */
	data->light0 = liren_light_new (data->scene, diffuse, equation, M_PI, 0.0f, 0);
	data->light1 = liren_light_new (data->scene, diffuse, equation, M_PI, 0.0f, 0);
	if (data->light0 == NULL || data->light1 == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	liren_lighting_insert_light (data->scene->lighting, data->light0);
	liren_lighting_insert_light (data->scene->lighting, data->light1);

	return self;
}

void
liext_preview_build (LIExtPreview* self)
{
	ligen_generator_rebuild_scene (self->generator);
}

int
liext_preview_build_block (LIExtPreview*   self,
                           LIVoxBlockAddr* addr)
{
	LIExtBlock* block;

	/* Find the block. */
	block = lialg_memdic_find (self->blocks, addr, sizeof (LIVoxBlockAddr));
	if (block == NULL)
	{
		block = liext_block_new (self);
		if (block == NULL)
			return 0;
		if (!lialg_memdic_insert (self->blocks, addr, sizeof (LIVoxBlockAddr), block))
		{
			liext_block_free (block);
			return 0;
		}
	}

	/* Build the block. */
	if (!liext_block_build (block, addr))
	{
		lialg_memdic_remove (self->blocks, addr, sizeof (LIVoxBlockAddr));
		liext_block_free (block);
		return 0;
	}

	return 1;
}

int
liext_preview_build_brush (LIExtPreview* self,
                           LIGenBrush*   brush,
                           int           object)
{
	int i;
	LIGenBrushobject* object_;
	LIMatQuaternion quat;
	LIMatTransform transform;
	LIMatVector vec;

	/* Create terrain. */
	liext_preview_clear (self);
	liext_preview_insert_stroke (self, 0, 0, 0, brush->id);

	/* Create objects. */
	for (i = 0 ; i < brush->objects.count ; i++)
	{
		object_ = brush->objects.array[i];
		if (object == i)
		{
			liext_preview_get_transform (self, &transform);
			vec = limat_vector_add (transform.position, object_->transform.position);
			quat = limat_quaternion_multiply (transform.rotation, object_->transform.rotation);
			transform = limat_transform_init (vec, quat);
		}
		else
			transform = object_->transform;
		liext_preview_insert_object (self, &transform, object_->model);
	}

	/* Rebuild. */
	liext_preview_build (self);

	return 1;
}

int
liext_preview_build_rule (LIExtPreview* self,
                          LIGenBrush*   brush,
                          LIGenRule*    rule,
                          int           stroke)
{
	int i;
	int j;
	int pos[3];
	LIGenBrushobject* object;
	LIGenBrush* brush_;
	LIGenRulestroke* stroke_;
	LIMatTransform transform;
	LIMatVector offset;

	/* Create rules. */
	liext_preview_clear (self);
	for (i = 0 ; i < rule->strokes.count ; i++)
	{
		stroke_ = rule->strokes.array + i;
		brush_ = ligen_generator_find_brush (self->generator, stroke_->brush);
		if (brush_ == NULL)
			continue;

		/* Create terrain. */
		pos[0] = stroke_->pos[0];
		pos[1] = stroke_->pos[1];
		pos[2] = stroke_->pos[2];
		if (stroke == i)
		{
			liext_preview_get_transform (self, &transform);
			pos[0] += (int)(round (transform.position.x / LIVOX_TILE_WIDTH));
			pos[1] += (int)(round (transform.position.y / LIVOX_TILE_WIDTH));
			pos[2] += (int)(round (transform.position.z / LIVOX_TILE_WIDTH));
		}
		liext_preview_insert_stroke (self, pos[0], pos[1], pos[2], stroke_->brush);

		/* Create objects. */
		for (j = 0 ; j < brush_->objects.count ; j++)
		{
			object = brush_->objects.array[j];
			offset = limat_vector_init (pos[0], pos[1], pos[2]);
			offset = limat_vector_multiply (offset, LIVOX_TILE_WIDTH);
			transform = limat_convert_vector_to_transform (offset);
			transform = limat_transform_multiply (transform, object->transform);
			liext_preview_insert_object (self, &transform, object->model);
		}
	}

	/* Create main terrain. */
	liext_preview_insert_stroke (self, 0, 0, 0, brush->id);

	/* Create main objects. */
	for (j = 0 ; j < brush->objects.count ; j++)
	{
		object = brush->objects.array[j];
		transform = object->transform;
		liext_preview_insert_object (self, &transform, object->model);
	}

	/* Rebuild. */
	liext_preview_build (self);

	return 1;
}

int
liext_preview_build_tile (LIExtPreview* self,
                          int           material)
{
	LIVoxVoxel voxel;

	livox_voxel_init (&voxel, material);
	lialg_sectors_clear (self->generator->voxels->sectors);
	livox_manager_set_voxel (self->generator->voxels,
		LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER, &voxel);
	livox_manager_update (self->generator->voxels, 1.0f);

	return 1;
}

int
liext_preview_clear (LIExtPreview* self)
{
	LIAlgPtrdicIter iter;
	LIAlgMemdicIter iter1;

	liren_group_clear (self->group);
	ligen_generator_clear_scene (self->generator);
	LIALG_PTRDIC_FOREACH (iter, self->objects)
		liren_object_free (iter.value);
	lialg_ptrdic_clear (self->objects);
	LIALG_MEMDIC_FOREACH (iter1, self->blocks)
		liext_block_free (iter1.value);
	lialg_memdic_clear (self->blocks);

	return 1;
}

void
liext_preview_copy_voxels (LIExtPreview* self,
                           int           startx,
                           int           starty,
                           int           startz,
                           int           sizex,
                           int           sizey,
                           int           sizez,
                           LIVoxVoxel*   result)
{
	livox_manager_copy_voxels (self->generator->voxels,
		LIEXT_PREVIEW_CENTER + startx,
		LIEXT_PREVIEW_CENTER + starty,
		LIEXT_PREVIEW_CENTER + startz,
		sizex, sizey, sizez, result);
}

/**
 * \brief Inserts an object to the preview widget.
 *
 * \param self Preview.
 * \param transform Transformation.
 * \param model Model name.
 * \return Nonzero on success.
 */
int
liext_preview_insert_object (LIExtPreview*         self,
                             const LIMatTransform* transform,
                             const char*           model)
{
	LIMatTransform t;
	LIMatVector v;
	LIEngModel* emodel;
	LIRenModel* rmodel;
	LIRenObject* object;

	/* Find model. */
	rmodel = liren_render_find_model (self->client->render, model);
	if (rmodel == NULL)
	{
		emodel = lieng_engine_find_model_by_name (self->client->engine, model);
		if (emodel == NULL)
			return 0;
		liren_render_load_model (self->client->render, model, emodel->model);
		rmodel = liren_render_find_model (self->client->render, model);
		if (rmodel == NULL)
			return 0;
	}

	/* Create object. */
	object = liren_object_new (self->scene, self->objects->size);
	if (object == NULL)
		return 0;
	v = limat_vector_init (LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER);
	v = limat_vector_multiply (v, LIVOX_TILE_WIDTH);
	t = limat_convert_vector_to_transform (v);
	t = limat_transform_multiply (t, *transform);
	liren_object_set_transform (object, &t);
	liren_object_set_model (object, rmodel);
	liren_object_set_realized (object, 1);

	/* Add to dictionary. */
	if (!lialg_ptrdic_insert (self->objects, object, object))
	{
		liren_object_free (object);
		return 0;
	}

	return 1;
}

int
liext_preview_insert_stroke (LIExtPreview* self,
                             int           x,
                             int           y,
                             int           z,
                             int           brush)
{
	return ligen_generator_insert_stroke (self->generator, brush,
		LIEXT_PREVIEW_CENTER + x,
		LIEXT_PREVIEW_CENTER + y,
		LIEXT_PREVIEW_CENTER + z);
}

/**
 * \brief Replaces the materials of the preview with those in the passed stream reader.
 *
 * \param self Preview.
 * \param reader Stream reader.
 * \return Nonzero on success.
 */
int
liext_preview_replace_materials (LIExtPreview* self,
                                 LIArcReader*  reader)
{
	LIVoxMaterial* material;

	livox_manager_clear_materials (self->generator->voxels);
	while (!liarc_reader_check_end (reader))
	{
		material = livox_material_new_from_stream (reader);
		if (material == NULL)
			return 0;
		if (!livox_manager_insert_material (self->generator->voxels, material))
		{
			livox_material_free (material);
			return 0;
		}
	}

	return 1;
}

void
liext_preview_setup_camera (LIExtPreview* self,
                            LIMatVector*  eye,
                            LIMatVector*  ctr,
                            LIMatVector*  up,
                            int           driver)
{
	LIMatTransform transform0;
	LIMatTransform transform1;

	transform0.rotation = limat_quaternion_identity ();
	transform0.position = limat_vector_init (
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f) + eye->x,
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f) + eye->y,
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f) + eye->z);
	transform1.rotation = limat_quaternion_look (limat_vector_subtract (*ctr, *eye), *up);
	transform1.position = transform0.position;
	lialg_camera_set_center (self->camera, &transform0);
	lialg_camera_set_driver (self->camera, driver);
	lialg_camera_set_transform (self->camera, &transform1);
	lialg_camera_warp (self->camera);
}

void
liext_preview_get_bounds (LIExtPreview* self,
                          LIMatAabb*    aabb)
{
	int i;
	int x[2];
	int y[2];
	int z[2];
	LIGenStroke* stroke;

	aabb->min = limat_vector_init (LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER);
	aabb->min = limat_vector_multiply (aabb->min, LIVOX_TILE_WIDTH);
	aabb->max = aabb->min;
	for (i = 0 ; i < self->generator->strokes.count ; i++)
	{
		stroke = self->generator->strokes.array + i;
		x[0] = LIVOX_TILE_WIDTH * (stroke->pos[0]);
		x[1] = LIVOX_TILE_WIDTH * (stroke->pos[0] + stroke->size[0]);
		y[0] = LIVOX_TILE_WIDTH * (stroke->pos[1]);
		y[1] = LIVOX_TILE_WIDTH * (stroke->pos[1] + stroke->size[1]);
		z[0] = LIVOX_TILE_WIDTH * (stroke->pos[2]);
		z[1] = LIVOX_TILE_WIDTH * (stroke->pos[2] + stroke->size[2]);
		if (aabb->min.x > x[0]) aabb->min.x = x[0];
		if (aabb->min.y > y[0]) aabb->min.y = y[0];
		if (aabb->min.z > z[0]) aabb->min.z = z[0];
		if (aabb->max.x < x[1]) aabb->max.x = x[1];
		if (aabb->max.y < y[1]) aabb->max.y = y[1];
		if (aabb->max.z < z[1]) aabb->max.z = z[1];
	}
}

void
liext_preview_get_camera_transform (LIExtPreview*   self,
                                    LIMatTransform* value)
{
	LIMatVector center;

	center = limat_vector_init (LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER);
	center = limat_vector_multiply (center, LIVOX_TILE_WIDTH);
	lialg_camera_get_transform (self->camera, value);
	value->position = limat_vector_subtract (value->position, center);
}

void
liext_preview_get_transform (LIExtPreview*   self,
                             LIMatTransform* value)
{
	*value = limat_transform_identity ();
}

/****************************************************************************/

static int
private_init (LIExtPreview* self,
              LIWdgManager* manager)
{
	LIWDG_RENDER (self)->custom_render_func = (void*) private_render_preview;
	LIWDG_RENDER (self)->custom_render_data = self;
	return 1;
}

static void
private_free (LIExtPreview* self)
{
	LIAlgPtrdicIter iter;
	LIAlgMemdicIter iter1;

	if (self->light0 != NULL)
	{
		liren_lighting_remove_light (self->scene->lighting, self->light0);
		liren_light_free (self->light0);
	}
	if (self->light1 != NULL)
	{
		liren_lighting_remove_light (self->scene->lighting, self->light1);
		liren_light_free (self->light1);
	}
	if (self->objects != NULL)
	{
		LIALG_PTRDIC_FOREACH (iter, self->objects)
			liren_object_free (iter.value);
		lialg_ptrdic_free (self->objects);
	}
	if (self->blocks != NULL)
	{
		LIALG_MEMDIC_FOREACH (iter1, self->blocks)
			liext_block_free (iter1.value);
		lialg_memdic_free (self->blocks);
	}
	if (self->group != NULL)
		liren_group_free (self->group);
	if (self->camera != NULL)
		lialg_camera_free (self->camera);
	if (self->generator != NULL)
	{
		lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
		ligen_generator_free (self->generator);
	}
	if (self->sectors != NULL)
		lialg_sectors_free (self->sectors);
	if (self->callbacks != NULL)
		lical_callbacks_free (self->callbacks);
	if (self->scene != NULL)
		liren_scene_free (self->scene);
}

static int
private_event (LIExtPreview* self,
               LIWdgEvent*   event)
{
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIMatTransform transform;
	LIWdgRect rect;

	if (event->type == LIWDG_EVENT_TYPE_BUTTON_PRESS)
	{
		if (liwdg_widget_get_grab (LIWDG_WIDGET (self)))
		{
			switch (event->button.button)
			{
				case 4:
					lialg_camera_move (self->camera, 5.0f);
					break;
				case 5:
					lialg_camera_move (self->camera, -5.0f);
					break;
			}
		}
		else
		{
			switch (event->button.button)
			{
				case 1:
					liwdg_widget_set_grab (LIWDG_WIDGET (self), 1);
					break;
				case 3:
					if (!lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "pressed", lical_marshal_DATA_INT_INT, event->button.x, event->button.y))
						return 0;
					break;
			}
		}
		return 0;
	}
	if (event->type == LIWDG_EVENT_TYPE_MOTION)
	{
		if (liwdg_widget_get_grab (LIWDG_WIDGET (self)))
			private_motion (self, event);
	}
	if (event->type == LIWDG_EVENT_TYPE_UPDATE)
	{
		/* Update camera. */
		liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
		lialg_camera_set_viewport (self->camera, rect.x, rect.y, rect.width, rect.height);
		lialg_camera_update (self->camera, event->update.secs);
		lialg_camera_get_modelview (self->camera, &modelview);
		lialg_camera_get_projection (self->camera, &projection);

		/* Update scene. */
		liwdg_render_set_modelview (LIWDG_RENDER (self), &modelview);
		liwdg_render_set_projection (LIWDG_RENDER (self), &projection);
		liren_scene_update (self->scene, event->update.secs);

		/* Setup lights. */
		lialg_camera_get_transform (self->camera, &transform);
		transform.position = limat_transform_transform (transform, limat_vector_init (9, 6, -1));
		liren_light_set_transform (self->light0, &transform);
		lialg_camera_get_transform (self->camera, &transform);
		transform.position = limat_transform_transform (transform, limat_vector_init (-4, 2, 0));
		liren_light_set_transform (self->light1, &transform);
	}

	return liwdg_widget_render ()->event (LIWDG_WIDGET (self), event);
}

static int
private_block_free (LIExtPreview*     self,
                    LIVoxUpdateEvent* event)
{
	LIExtBlock* block;
	LIVoxBlockAddr addr;

	addr.sector[0] = event->sector[0];
	addr.sector[1] = event->sector[1];
	addr.sector[2] = event->sector[2];
	addr.block[0] = event->block[0];
	addr.block[1] = event->block[1];
	addr.block[2] = event->block[2];
	block = lialg_memdic_find (self->blocks, &addr, sizeof (addr));
	if (block != NULL)
	{
		lialg_memdic_remove (self->blocks, &addr, sizeof (addr));
		liext_block_free (block);
	}

	return 1;
}

static int
private_block_load (LIExtPreview*     self,
                    LIVoxUpdateEvent* event)
{
	LIVoxBlockAddr addr;

	addr.sector[0] = event->sector[0];
	addr.sector[1] = event->sector[1];
	addr.sector[2] = event->sector[2];
	addr.block[0] = event->block[0];
	addr.block[1] = event->block[1];
	addr.block[2] = event->block[2];
	liext_preview_build_block (self, &addr);

	return 1;
}

static void
private_motion (LIExtPreview* self,
                LIWdgEvent*   event)
{
	LIMatVector vx;
	LIMatVector vy;
	LIMatTransform transform;
	LIMatTransform transform1;

	self->drag.x += event->motion.dx;
	self->drag.y += event->motion.dy;
	if (event->motion.buttons & 0x02)
	{
		lialg_camera_get_transform (self->camera, &transform);
		vx = limat_quaternion_transform (transform.rotation, limat_vector_init (1.0f, 0.0f, 0.0f));
		vy = limat_quaternion_transform (transform.rotation, limat_vector_init (0.0f, -1.0f, 0.0f));
		vx = limat_vector_multiply (vx, LIEXT_PREVIEW_PAN * event->motion.dx);
		vy = limat_vector_multiply (vy, LIEXT_PREVIEW_PAN * event->motion.dy);
		transform1 = limat_convert_vector_to_transform (limat_vector_add (vx, vy));
		transform1 = limat_transform_multiply (transform1, transform);
		lialg_camera_set_transform (self->camera, &transform1);
	}
	else
	{
		lialg_camera_turn (self->camera, -0.01 * event->motion.dx);
		lialg_camera_tilt (self->camera, -0.01 * event->motion.dy);
	}
}

static void
private_render_preview (LIWdgWidget*  widget,
                        LIExtPreview* self)
{
	int i;
	int x[2];
	int y[2];
	int z[2];
	LIGenStroke* stroke;

	/* Render stroke bounds. */
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_TEXTURE_2D);
	glBegin (GL_LINES);
	for (i = 0 ; i < self->generator->strokes.count ; i++)
	{
		stroke = self->generator->strokes.array + i;
		if (i == self->generator->strokes.count - 1)
			glColor3f (1.0f, 0.0f, 0.0f);
		else
			glColor3f (0.0f, 1.0f, 0.0f);
		x[0] = LIVOX_TILE_WIDTH * (stroke->pos[0]);
		x[1] = LIVOX_TILE_WIDTH * (stroke->pos[0] + stroke->size[0]);
		y[0] = LIVOX_TILE_WIDTH * (stroke->pos[1]);
		y[1] = LIVOX_TILE_WIDTH * (stroke->pos[1] + stroke->size[1]);
		z[0] = LIVOX_TILE_WIDTH * (stroke->pos[2]);
		z[1] = LIVOX_TILE_WIDTH * (stroke->pos[2] + stroke->size[2]);
		glVertex3f (x[0], y[0], z[0]);
		glVertex3f (x[1], y[0], z[0]);
		glVertex3f (x[1], y[0], z[0]);
		glVertex3f (x[1], y[0], z[1]);
		glVertex3f (x[1], y[0], z[1]);
		glVertex3f (x[0], y[0], z[1]);
		glVertex3f (x[0], y[0], z[1]);
		glVertex3f (x[0], y[0], z[0]);

		glVertex3f (x[0], y[1], z[0]);
		glVertex3f (x[1], y[1], z[0]);
		glVertex3f (x[1], y[1], z[0]);
		glVertex3f (x[1], y[1], z[1]);
		glVertex3f (x[1], y[1], z[1]);
		glVertex3f (x[0], y[1], z[1]);
		glVertex3f (x[0], y[1], z[1]);
		glVertex3f (x[0], y[1], z[0]);

		glVertex3f (x[0], y[0], z[0]);
		glVertex3f (x[0], y[1], z[0]);
		glVertex3f (x[1], y[0], z[0]);
		glVertex3f (x[1], y[1], z[0]);
		glVertex3f (x[0], y[0], z[1]);
		glVertex3f (x[0], y[1], z[1]);
		glVertex3f (x[1], y[0], z[1]);
		glVertex3f (x[1], y[1], z[1]);
	}
	glEnd ();
}

/** @} */
/** @} */
/** @} */

/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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

#include "ext-preview.h"

#define LIEXT_PREVIEW_CENTER 8160

static const void*
private_base ();

static int
private_init (liextPreview* self,
              liwdgManager* manager);

static void
private_free (liextPreview* self);

static int
private_event (liextPreview* self,
               liwdgEvent*   event);

static void
private_render_preview (liwdgWidget*  widget,
                        liextPreview* self);

/****************************************************************************/

const liwdgClass liextPreviewType =
{
	LIWDG_BASE_DYNAMIC, private_base, "GeneratorPreview", sizeof (liextPreview),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

liwdgWidget*
liext_preview_new (liwdgManager* manager,
                   licliModule*  module)
{
	const float diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float equation[3] = { 1.0f, 0.0f, 0.001f };
	liextPreview* data;
	limatTransform transform;
	liwdgWidget* self;

	/* Allocate self. */
	self = liwdg_widget_new (manager, &liextPreviewType);
	if (self == NULL)
		return NULL;
	data = LIEXT_PREVIEW (self);
	data->module = module;
	data->render = module->engine->render;

	/* Allocate scene. */
	data->scene = lirnd_scene_new (data->render);
	if (data->scene == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	LIWDG_RENDER (self)->scene = data->scene;

	/* Allocate generator. */
	data->generator = ligen_generator_new_full (module->paths->root,
		module->name, data->scene, module->engine->renderapi);
	if (data->generator == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	ligen_generator_set_fill (data->generator, 0);

	/* Create camera. */
	data->camera = lieng_camera_new (module->engine);
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
	lieng_camera_set_clip (data->camera, 0);
	lieng_camera_set_driver (data->camera, LIENG_CAMERA_DRIVER_MANUAL);
	lieng_camera_set_transform (data->camera, &transform);
	lieng_camera_warp (data->camera);

	/* Create lights. */
	data->light0 = lirnd_light_new (data->scene, diffuse, equation, M_PI, 0.0f, 0);
	data->light1 = lirnd_light_new (data->scene, diffuse, equation, M_PI, 0.0f, 0);
	if (data->light0 == NULL || data->light1 == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	lirnd_lighting_insert_light (data->scene->lighting, data->light0);
	lirnd_lighting_insert_light (data->scene->lighting, data->light1);

	return self;
}

void
liext_preview_build (liextPreview* self)
{
	ligen_generator_rebuild_scene (self->generator);
}

int
liext_preview_clear (liextPreview* self)
{
	lialgU32dicIter iter;
	livoxMaterial* material;

	/* Clear scene and materials. */
	ligen_generator_clear_scene (self->generator);
	livox_manager_clear_materials (self->generator->voxels);

	/* Copy materials. */
	LI_FOREACH_U32DIC (iter, self->module->voxels->materials)
	{
		material = livox_material_new_copy (iter.value);
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

int
liext_preview_insert_stroke (liextPreview* self,
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

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgRenderType;
}

static int
private_init (liextPreview* self,
              liwdgManager* manager)
{
	LIWDG_RENDER (self)->custom_render_func = (void*) private_render_preview;
	LIWDG_RENDER (self)->custom_render_data = self;
	return 1;
}

static void
private_free (liextPreview* self)
{
	if (self->light0 != NULL)
	{
		lirnd_lighting_remove_light (self->scene->lighting, self->light0);
		lirnd_light_free (self->light0);
	}
	if (self->light1 != NULL)
	{
		lirnd_lighting_remove_light (self->scene->lighting, self->light1);
		lirnd_light_free (self->light1);
	}
	if (self->camera != NULL)
		lieng_camera_free (self->camera);
	if (self->generator != NULL)
		ligen_generator_free (self->generator);
	if (self->scene != NULL)
		lirnd_scene_free (self->scene);
}

static int
private_event (liextPreview* self,
               liwdgEvent*   event)
{
	limatMatrix modelview;
	limatMatrix projection;
	limatTransform transform;
	liwdgRect rect;

	if (event->type == LIWDG_EVENT_TYPE_BUTTON_PRESS)
	{
		lieng_camera_get_transform (self->camera, &transform);
		switch (event->button.button)
		{
			case 4:
				lieng_camera_move (self->camera, 5.0f);
				break;
			case 5:
				lieng_camera_move (self->camera, -5.0f);
				break;
		}
		return 0;
	}
	if (event->type == LIWDG_EVENT_TYPE_MOTION)
	{
		if (event->motion.buttons & 0x1)
		{
			lieng_camera_turn (self->camera, -0.01 * event->motion.dx);
			lieng_camera_tilt (self->camera, 0.01 * event->motion.dy);
		}
	}
	if (event->type == LIWDG_EVENT_TYPE_UPDATE)
	{
		/* Update camera. */
		liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
		lieng_camera_set_viewport (self->camera, rect.x, rect.y, rect.width, rect.height);
		lieng_camera_update (self->camera, event->update.secs);
		lieng_camera_get_modelview (self->camera, &modelview);
		lieng_camera_get_projection (self->camera, &projection);

		/* Update scene. */
		liwdg_render_set_modelview (LIWDG_RENDER (self), &modelview);
		liwdg_render_set_projection (LIWDG_RENDER (self), &projection);
		lirnd_scene_update (self->scene, event->update.secs);

		/* Setup lights. */
		lieng_camera_get_transform (self->camera, &transform);
		transform.position = limat_transform_transform (transform, limat_vector_init (9, 6, -1));
		lirnd_light_set_transform (self->light0, &transform);
		lieng_camera_get_transform (self->camera, &transform);
		transform.position = limat_transform_transform (transform, limat_vector_init (-4, 2, 0));
		lirnd_light_set_transform (self->light1, &transform);
	}

	return liwdgRenderType.event (LIWDG_WIDGET (self), event);
}

static void
private_render_preview (liwdgWidget*  widget,
                        liextPreview* self)
{
	int i;
	int x[2];
	int y[2];
	int z[2];
	ligenStroke* stroke;

	/* Render stroke bounds. */
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_LIGHTING);
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

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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndDraw Draw
 * @{
 */

#include "render-context.h"
#include "render-draw.h"

void
lirnd_draw_bounds (lirndContext* context,
                   lirndObject*  object,
                   void*         data)
{
	limatAabb aabb;

	/* Check if renderable. */
	if (!lirnd_object_get_realized (object))
		return;

	/* Frustum culling. */
	lirnd_object_get_bounds (object, &aabb);
	if (limat_frustum_cull_aabb (&context->frustum, &aabb))
		return;

	/* Render bounds. */
	glBegin (GL_LINE_LOOP);
	glVertex3f (aabb.min.x, aabb.min.y, aabb.min.z);
	glVertex3f (aabb.max.x, aabb.min.y, aabb.min.z);
	glVertex3f (aabb.max.x, aabb.min.y, aabb.max.z);
	glVertex3f (aabb.min.x, aabb.min.y, aabb.max.z);
	glEnd ();
	glBegin (GL_LINE_LOOP);
	glVertex3f (aabb.min.x, aabb.max.y, aabb.min.z);
	glVertex3f (aabb.max.x, aabb.max.y, aabb.min.z);
	glVertex3f (aabb.max.x, aabb.max.y, aabb.max.z);
	glVertex3f (aabb.min.x, aabb.max.y, aabb.max.z);
	glEnd ();
	glBegin (GL_LINES);
	glVertex3f (aabb.min.x, aabb.min.y, aabb.min.z);
	glVertex3f (aabb.min.x, aabb.max.y, aabb.min.z);
	glVertex3f (aabb.max.x, aabb.min.y, aabb.min.z);
	glVertex3f (aabb.max.x, aabb.max.y, aabb.min.z);
	glVertex3f (aabb.max.x, aabb.min.y, aabb.max.z);
	glVertex3f (aabb.max.x, aabb.max.y, aabb.max.z);
	glVertex3f (aabb.min.x, aabb.min.y, aabb.max.z);
	glVertex3f (aabb.min.x, aabb.max.y, aabb.max.z);
	glEnd ();

#ifdef LIRND_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

void
lirnd_draw_exclude (lirndContext* context,
                    lirndObject*  object,
                    void*         data)
{
	int i;
	int flags;
	limatAabb aabb;
	limatMatrix matrix;
	limatVector center;
	lirndMaterial* material;

	/* Check if renderable. */
	if (!lirnd_object_get_realized (object))
		return;

	/* Exlude object. */
	if ((lirndObject*) data == object)
		return;

	/* Frustum culling. */
	lirnd_object_get_bounds (object, &aabb);
	if (limat_frustum_cull_aabb (&context->frustum, &aabb))
		return;

	/* Lighting. */
	lirnd_object_get_center (object, &center);
	lirnd_scene_set_light_focus (context->scene, &center);

	/* Rendering mode. */
	flags = !context->render->shader.enabled? LIRND_FLAG_FIXED : 0;
	flags |= LIRND_FLAG_LIGHTING;
	flags |= LIRND_FLAG_TEXTURING;
	flags |= context->render->config.global_shadows? LIRND_FLAG_SHADOW0 : 0;
	flags |= context->render->config.local_shadows? LIRND_FLAG_SHADOW1 : 0;

	/* Render the mesh. */
	matrix = object->orientation.matrix;
	for (i = 0 ; i < object->buffers.count ; i++)
	{
		material = object->buffers.array[i].material;
		lirnd_context_set_flags (context, flags);
		lirnd_context_set_lights (context,
			context->scene->lighting->active_lights.array,
			context->scene->lighting->active_lights.count);
		lirnd_context_set_matrix (context, &matrix);
		lirnd_context_set_material (context, material);
		lirnd_context_set_shader (context, material->shader);
		lirnd_context_set_textures (context, material->textures.array, material->textures.count);
		lirnd_context_bind (context);
		lirnd_context_render (context, object->buffers.array + i);
	}

#ifdef LIRND_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

void
lirnd_draw_hair (lirndContext* context,
                 lirndObject*  object,
                 void*         data)
{
	int i;
	int j;
	int k;
	int flags;
	float w;
	float sx;
	float sy;
	float len0;
	float len1;
	float blend;
	limatAabb aabb;
	limatMatrix matrix;
	limatVector bbx;
	limatVector bby;
	limatVector bbz;
	limatVector ctr;
	limatVector dir;
	limatVector tmp;
	limatVector center;
	limatVector coord[4];
	limdlHair* hair;
	limdlHairs* hairs;
	lirndMaterial* material;

	/* Check if renderable. */
	if (!lirnd_object_get_realized (object) || object->model == NULL)
		return;

	/* Frustum culling. */
	lirnd_object_get_bounds (object, &aabb);
	if (limat_frustum_cull_aabb (&context->frustum, &aabb))
		return;

	/* Lighting. */
	lirnd_object_get_center (object, &center);
	lirnd_scene_set_light_focus (context->scene, &center);

	/* Rendering mode. */
	flags = !context->render->shader.enabled? LIRND_FLAG_FIXED : 0;
	flags |= LIRND_FLAG_LIGHTING;
	flags |= LIRND_FLAG_TEXTURING;
	flags |= context->render->config.global_shadows? LIRND_FLAG_SHADOW0 : 0;
	flags |= context->render->config.local_shadows? LIRND_FLAG_SHADOW1 : 0;
	matrix = object->orientation.matrix;
	lirnd_context_set_flags (context, flags);
	lirnd_context_set_matrix (context, &matrix);
	lirnd_context_set_lights (context,
		context->scene->lighting->active_lights.array,
		context->scene->lighting->active_lights.count);

	/* Calculate billboard axis. */
	bbx = limat_vector_init (context->modelview.m[0], context->modelview.m[4], context->modelview.m[8]);
	bby = limat_vector_init (context->modelview.m[1], context->modelview.m[5], context->modelview.m[9]);
	bbz = limat_vector_init (context->modelview.m[2], context->modelview.m[6], context->modelview.m[10]);

	/* Render hair groups. */
	glDisable (GL_CULL_FACE);
	glNormal3f (bbz.x, bbz.y, bbz.z);
	for (i = 0 ; i < object->model->model->hairs.count ; i++)
	{
		hairs = object->model->model->hairs.array + i;
		assert (hairs->material >= 0);
		assert (hairs->material < object->materials.count);

		/* Bind hair group material. */
		material = object->materials.array[hairs->material];
		lirnd_context_set_material (context, material);
		lirnd_context_set_shader (context, material->shader);
		lirnd_context_set_textures (context, material->textures.array, material->textures.count);
		lirnd_context_bind (context);

		/* Render each hair as billboard. */
		for (j = 0 ; j < hairs->count ; j++)
		{
			hair = hairs->hairs + j;
			len0 = len1 = 0.01f;
			for (k = 1 ; k < hair->count ; k++)
			{
				len1 += limat_vector_get_length (limat_vector_subtract (
					hair->nodes[k - 1].position, hair->nodes[k].position));
			}
			glBegin (GL_TRIANGLE_STRIP);
			for (k = 0 ; k < hair->count ; k++)
			{
				/* FIXME: This is a rather lousy way to do billboarding. */
				if (k > 0)
				{
					len0 += limat_vector_get_length (limat_vector_subtract (
						hair->nodes[k - 1].position, hair->nodes[k].position));
				}
				w = material->strand_start * (1.0 - len0 / len1) +
				    material->strand_end * (len0 / len1);
				sx = 4.5f; /* FIXME */
				sy = 1.5f; /* FIXME */
				ctr = limat_matrix_transform (matrix, hair->nodes[k].position);
				if (!k)
					dir = limat_vector_subtract (hair->nodes[k].position, hair->nodes[k + 1].position);
				else
					dir = limat_vector_subtract (hair->nodes[k - 1].position, hair->nodes[k].position);
				dir = limat_vector_normalize (dir);
				blend = LI_ABS (limat_vector_dot (dir, bby));
				tmp = limat_vector_multiply (bbx, blend);
				tmp = limat_vector_add (tmp, limat_vector_multiply (bby, 1.0f - blend));
				tmp = limat_vector_normalize (tmp);
				tmp = limat_vector_multiply (tmp, w);
				coord[0] = limat_vector_subtract (ctr, tmp);
				coord[1] = limat_vector_add (ctr, tmp);
				glTexCoord2f (0.5f - w * sx, sy * len0);
				glVertex3f (coord[0].x, coord[0].y, coord[0].z);
				glTexCoord2f (0.5f + w * sx, sy * len0);
				glVertex3f (coord[1].x, coord[1].y, coord[1].z);
			}
			glEnd ();
		}
	}

#ifdef LIRND_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

void
lirnd_draw_opaque (lirndContext* context,
                   lirndObject*  object,
                   void*         data)
{
	int i;
	int flags;
	limatAabb aabb;
	limatMatrix matrix;
	limatVector center;
	lirndMaterial* material;

	/* Check if renderable. */
	if (!lirnd_object_get_realized (object))
		return;

	/* Frustum culling. */
	lirnd_object_get_bounds (object, &aabb);
	if (limat_frustum_cull_aabb (&context->frustum, &aabb))
		return;

	/* Lighting. */
	lirnd_object_get_center (object, &center);
	lirnd_scene_set_light_focus (context->scene, &center);

	/* Rendering mode. */
	flags = !context->render->shader.enabled? LIRND_FLAG_FIXED : 0;
	flags |= LIRND_FLAG_LIGHTING;
	flags |= LIRND_FLAG_TEXTURING;
	flags |= context->render->config.global_shadows? LIRND_FLAG_SHADOW0 : 0;
	flags |= context->render->config.local_shadows? LIRND_FLAG_SHADOW1 : 0;

	/* Render the mesh. */
	matrix = object->orientation.matrix;
	for (i = 0 ; i < object->buffers.count ; i++)
	{
		material = object->buffers.array[i].material;
		if (!(material->flags & LIRND_MATERIAL_FLAG_TRANSPARENCY))
		{
			lirnd_context_set_flags (context, flags);
			lirnd_context_set_lights (context,
				context->scene->lighting->active_lights.array,
				context->scene->lighting->active_lights.count);
			lirnd_context_set_material (context, material);
			lirnd_context_set_matrix (context, &matrix);
			lirnd_context_set_shader (context, material->shader);
			lirnd_context_set_textures (context, material->textures.array, material->textures.count);
			lirnd_context_bind (context);
			lirnd_context_render (context, object->buffers.array + i);
		}
	}

#ifdef LIRND_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

void
lirnd_draw_picking (lirndContext* context,
                    lirndObject*  object,
                    void*         data)
{
	int i;
	int flags;
	limatAabb aabb;
	limatMatrix matrix;
	lirndMaterial* material;

	/* Check if renderable. */
	if (!lirnd_object_get_realized (object))
		return;

	/* Frustum culling. */
	lirnd_object_get_bounds (object, &aabb);
	if (limat_frustum_cull_aabb (&context->frustum, &aabb))
		return;

	/* Rendering mode. */
	flags = LIRND_FLAG_FIXED;
	glLoadName (object->id);

	/* Render the mesh. */
	matrix = object->orientation.matrix;
	for (i = 0 ; i < object->buffers.count ; i++)
	{
		material = object->buffers.array[i].material;
		lirnd_context_set_flags (context, flags);
		lirnd_context_set_matrix (context, &matrix);
		lirnd_context_bind (context);
		lirnd_context_render (context, object->buffers.array + i);
	}
}

void
lirnd_draw_shadeless (lirndContext* context,
                      lirndObject*  object,
                      void*         data)
{
	int i;
	int flags;
//	limatAabb aabb;
	limatMatrix matrix;
	lirndMaterial* material;

	/* Check if renderable. */
	if (!lirnd_object_get_realized (object))
		return;

	/* Frustum culling. */
/*	lirnd_object_get_bounds (object, &aabb);
	if (limat_frustum_cull_aabb (context->frustum, &aabb))
		return;*/

	/* Rendering mode. */
	flags = !context->render->shader.enabled? LIRND_FLAG_FIXED : 0;
	flags |= LIRND_FLAG_TEXTURING;

	/* Render the mesh. */
	matrix = limat_matrix_identity ();
	for (i = 0 ; i < object->buffers.count ; i++)
	{
		material = object->buffers.array[i].material;
		lirnd_context_set_flags (context, flags);
		lirnd_context_set_material (context, material);
		lirnd_context_set_matrix (context, &matrix);
		lirnd_context_set_shader (context, material->shader);
		lirnd_context_set_textures (context, material->textures.array, material->textures.count);
		lirnd_context_bind (context);
		lirnd_context_render (context, object->buffers.array + i);
	}

#ifdef LIRND_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

void
lirnd_draw_shadowmap (lirndContext* context,
                      lirndObject*  object,
                      void*         data)
{
	int i;
	limatAabb aabb;
	limatMatrix matrix;

	/* Check if renderable. */
	if (!lirnd_object_get_realized (object))
		return;

	/* Frustum culling. */
	lirnd_object_get_bounds (object, &aabb);
	if (limat_frustum_cull_aabb (&context->frustum, &aabb))
		return;

	/* Render the mesh. */
	matrix = object->orientation.matrix;
	for (i = 0 ; i < object->buffers.count ; i++)
	{
		lirnd_context_set_flags (context, LIRND_FLAG_FIXED);
		lirnd_context_set_matrix (context, &matrix);
		lirnd_context_set_shader (context, context->render->shader.shadowmap);
		lirnd_context_bind (context);
		lirnd_context_render (context, object->buffers.array + i);
	}

#ifdef LIRND_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

void
lirnd_draw_transparent (lirndContext* context,
                        lirndObject*  object,
                        void*         data)
{
	int i;
	int flags;
	limatAabb aabb;
	limatMatrix matrix;
	limatVector center;
	lirndMaterial* material;

	/* Check if renderable. */
	if (!lirnd_object_get_realized (object))
		return;

	/* Frustum culling. */
	lirnd_object_get_bounds (object, &aabb);
	if (limat_frustum_cull_aabb (&context->frustum, &aabb))
		return;

	/* Lighting. */
	lirnd_object_get_center (object, &center);
	lirnd_scene_set_light_focus (context->scene, &center);

	/* Rendering mode. */
	flags = !context->render->shader.enabled? LIRND_FLAG_FIXED : 0;
	flags |= LIRND_FLAG_LIGHTING;
	flags |= LIRND_FLAG_TEXTURING;
	flags |= context->render->config.global_shadows? LIRND_FLAG_SHADOW0 : 0;
	flags |= context->render->config.local_shadows? LIRND_FLAG_SHADOW1 : 0;

	/* Render the mesh. */
	matrix = object->orientation.matrix;
	for (i = 0 ; i < object->buffers.count ; i++)
	{
		material = object->buffers.array[i].material;
		if (material->flags & LIRND_MATERIAL_FLAG_TRANSPARENCY)
		{
			lirnd_context_set_flags (context, flags);
			lirnd_context_set_lights (context,
				context->scene->lighting->active_lights.array,
				context->scene->lighting->active_lights.count);
			lirnd_context_set_matrix (context, &matrix);
			lirnd_context_set_material (context, material);
			lirnd_context_set_shader (context, material->shader);
			lirnd_context_set_textures (context, material->textures.array, material->textures.count);
			lirnd_context_bind (context);
			lirnd_context_render (context, object->buffers.array + i);
		}
	}

#ifdef LIRND_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

/** @} */
/** @} */

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
lirnd_draw_debug (lirndContext* context,
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

	/* Render debug. */
	lirnd_object_render_debug (object, context->render);

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
	lirnd_render_set_light_focus (context->render, &center);

	/* Rendering mode. */
	flags = !context->render->shader.enabled? LIRND_FLAG_FIXED : 0;
	flags |= LIRND_FLAG_LIGHTING;
	flags |= LIRND_FLAG_TEXTURING;
	flags |= context->render->lighting->config.global_shadows? LIRND_FLAG_SHADOW0 : 0;
	flags |= context->render->lighting->config.local_shadows? LIRND_FLAG_SHADOW1 : 0;

	/* Render the mesh. */
	matrix = object->orientation.matrix;
	for (i = 0 ; i < object->materials.count ; i++)
	{
		material = object->materials.array[i];
		lirnd_context_set_flags (context, flags);
		lirnd_context_set_lights (context,
			context->render->lighting->active_lights.array,
			context->render->lighting->active_lights.count);
		lirnd_context_set_matrix (context, &matrix);
		lirnd_context_set_material (context, material);
		lirnd_context_set_shader (context, material->shader);
		lirnd_context_set_textures (context, material->textures.array, material->textures.count);
		lirnd_context_bind (context);
		lirnd_object_render_group (object, context, i);
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
	lirnd_render_set_light_focus (context->render, &center);

	/* Rendering mode. */
	flags = !context->render->shader.enabled? LIRND_FLAG_FIXED : 0;
	flags |= LIRND_FLAG_LIGHTING;
	flags |= LIRND_FLAG_TEXTURING;
	flags |= context->render->lighting->config.global_shadows? LIRND_FLAG_SHADOW0 : 0;
	flags |= context->render->lighting->config.local_shadows? LIRND_FLAG_SHADOW1 : 0;

	/* Render the mesh. */
	matrix = object->orientation.matrix;
	for (i = 0 ; i < object->materials.count ; i++)
	{
		material = object->materials.array[i];
		if (!(material->flags & LIRND_MATERIAL_FLAG_TRANSPARENCY))
		{
			lirnd_context_set_flags (context, flags);
			lirnd_context_set_lights (context,
				context->render->lighting->active_lights.array,
				context->render->lighting->active_lights.count);
			lirnd_context_set_material (context, material);
			lirnd_context_set_matrix (context, &matrix);
			lirnd_context_set_shader (context, material->shader);
			lirnd_context_set_textures (context, material->textures.array, material->textures.count);
			lirnd_context_bind (context);
			lirnd_object_render_group (object, context, i);
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
	for (i = 0 ; i < object->materials.count ; i++)
	{
		material = object->materials.array[i];
		lirnd_context_set_flags (context, flags);
		lirnd_context_set_matrix (context, &matrix);
		lirnd_context_bind (context);
		lirnd_object_render_group (object, context, i);
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
	for (i = 0 ; i < object->materials.count ; i++)
	{
		material = object->materials.array[i];
		lirnd_context_set_flags (context, flags);
		lirnd_context_set_material (context, material);
		lirnd_context_set_matrix (context, &matrix);
		lirnd_context_set_shader (context, material->shader);
		lirnd_context_set_textures (context, material->textures.array, material->textures.count);
		lirnd_context_bind (context);
		lirnd_object_render_group (object, context, i);
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
	for (i = 0 ; i < object->materials.count ; i++)
	{
		lirnd_context_set_flags (context, LIRND_FLAG_FIXED);
		lirnd_context_set_matrix (context, &matrix);
		lirnd_context_set_shader (context, context->render->shader.shadowmap);
		lirnd_context_bind (context);
		lirnd_object_render_group (object, context, i);
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
	lirnd_render_set_light_focus (context->render, &center);

	/* Rendering mode. */
	flags = !context->render->shader.enabled? LIRND_FLAG_FIXED : 0;
	flags |= LIRND_FLAG_LIGHTING;
	flags |= LIRND_FLAG_TEXTURING;
	flags |= context->render->lighting->config.global_shadows? LIRND_FLAG_SHADOW0 : 0;
	flags |= context->render->lighting->config.local_shadows? LIRND_FLAG_SHADOW1 : 0;

	/* Render the mesh. */
	matrix = object->orientation.matrix;
	for (i = 0 ; i < object->materials.count ; i++)
	{
		material = object->materials.array[i];
		if (material->flags & LIRND_MATERIAL_FLAG_TRANSPARENCY)
		{
			lirnd_context_set_flags (context, flags);
			lirnd_context_set_lights (context,
				context->render->lighting->active_lights.array,
				context->render->lighting->active_lights.count);
			lirnd_context_set_matrix (context, &matrix);
			lirnd_context_set_material (context, material);
			lirnd_context_set_shader (context, material->shader);
			lirnd_context_set_textures (context, material->textures.array, material->textures.count);
			lirnd_context_bind (context);
			lirnd_object_render_group (object, context, i);
		}
	}

#ifdef LIRND_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

/** @} */
/** @} */

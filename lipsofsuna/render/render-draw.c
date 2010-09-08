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
 * \addtogroup liren Render
 * @{
 * \addtogroup lirenDraw Draw
 * @{
 */

#include "render-context.h"
#include "render-draw.h"

#ifdef LIMDL_DEBUG_ARMATURE
static void
private_draw_node (LIMdlNode* node)
{
	int i;

	glColor3f (1.0f, 1.0f, 0.0f);
	if (node->type == LIMDL_NODE_BONE)
	{
		glVertex3f (node->transform.global.position.x,
					node->transform.global.position.y,
					node->transform.global.position.z);
		glVertex3f (node->bone.tail.x,
					node->bone.tail.y,
					node->bone.tail.z);
	}
	else
	{
		glColor3f (0.0f, 1.0f, 0.0f);
		glVertex3f (node->transform.global.position.x,
					node->transform.global.position.y,
					node->transform.global.position.z);
		glVertex3f (node->transform.global.position.x,
					node->transform.global.position.y + 0.2,
					node->transform.global.position.z);
	}

	for (i = 0 ; i < node->nodes.count ; i++)
		private_draw_node (node->nodes.array[i]);
}
#endif

/*****************************************************************************/

void liren_draw_default (
	LIRenContext*  context,
	int            index,
	int            count,
	LIMatMatrix*   matrix,
	LIRenMaterial* material,
	LIRenBuffer*   buffer)
{
	if (context->deferred && material->shader_deferred == NULL)
		return;
	liren_context_set_flags (context, LIREN_FLAG_LIGHTING | LIREN_FLAG_TEXTURING);
	liren_context_set_material (context, material);
	liren_context_set_material_shader (context, material);
	liren_context_set_modelmatrix (context, matrix);
	liren_context_set_textures (context, material->textures.array, material->textures.count);
	liren_context_set_buffer (context, buffer);
	liren_context_bind (context);
	liren_context_render_indexed (context, index, count);
}

void
liren_draw_bounds (LIRenContext* context,
                   LIRenObject*  object,
                   void*         data)
{
	LIMatAabb aabb;

	/* Render bounds. */
	liren_object_get_bounds (object, &aabb);
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

#ifdef LIREN_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

void
liren_draw_debug (LIRenContext* context,
                  LIRenObject*  object,
                  void*         data)
{
#if defined LIMDL_DEBUG_ARMATURE
	int i;

	/* Check if renderable. */
	if (object->model == NULL || object->pose == NULL)
		return;

	glDisable (GL_DEPTH_TEST);

	/* Render armature. */
	glPushMatrix ();
	glMultMatrixf (object->orientation.matrix.m);
	glBegin (GL_LINES);
	for (i = 0 ; i < object->pose->nodes.count ; i++)
		private_draw_node (object->pose->nodes.array[i]);
	glEnd ();
	glPopMatrix ();
#endif
}

void
liren_draw_exclude (LIRenContext* context,
                    LIRenObject*  object,
                    void*         data)
{
	int i;
	int flags;
	LIRenMaterial* material;
	LIRenModel* model;

	model = object->model;

	/* Exlude object. */
	if ((LIRenObject*) data == object)
		return;

	/* Rendering mode. */
	flags = 0;
	flags |= LIREN_FLAG_LIGHTING;
	flags |= LIREN_FLAG_TEXTURING;
	liren_context_set_buffer (context, model->buffer);
	liren_context_set_flags (context, flags);
	liren_context_set_modelmatrix (context, &object->orientation.matrix);

	/* Render the mesh. */
	for (i = 0 ; i < model->groups.count ; i++)
	{
		material = model->materials.array[i];
		liren_context_set_material (context, material);
		liren_context_set_material_shader (context, material);
		liren_context_set_textures (context, material->textures.array, material->textures.count);
		liren_context_bind (context);
		liren_context_render_indexed (context, model->groups.array[i].start, model->groups.array[i].count);
	}

#ifdef LIREN_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

void
liren_draw_hair (LIRenContext* context,
                 LIRenObject*  object,
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
	LIMatMatrix matrix;
	LIMatMatrix view;
	LIMatVector bbx;
	LIMatVector bby;
	LIMatVector bbz;
	LIMatVector ctr;
	LIMatVector dir;
	LIMatVector tmp;
	LIMatVector coord[4];
	LIMdlHair* hair;
	LIMdlHairs* hairs;
	LIRenMaterial* material;
	LIRenModel* model;

	/* Check if renderable. */
	if (object->model == NULL)
		return;
	model = object->model;

	/* Rendering mode. */
	flags = 0;
	flags |= LIREN_FLAG_LIGHTING;
	flags |= LIREN_FLAG_TEXTURING;
	matrix = object->orientation.matrix;
	liren_context_set_flags (context, flags);
	liren_context_set_modelmatrix (context, &matrix);

	/* Calculate billboard axis. */
	view = context->matrix.view;
	bbx = limat_vector_init (view.m[0], view.m[4], view.m[8]);
	bby = limat_vector_init (view.m[1], view.m[5], view.m[9]);
	bbz = limat_vector_init (view.m[2], view.m[6], view.m[10]);

	/* Render hair groups. */
	glDisable (GL_CULL_FACE);
	glNormal3f (bbz.x, bbz.y, bbz.z);
	for (i = 0 ; i < object->model->model->hairs.count ; i++)
	{
		hairs = object->model->model->hairs.array + i;
		lisys_assert (hairs->material >= 0);
		lisys_assert (hairs->material < model->materials.count);

		/* Bind hair group material. */
		material = model->materials.array[hairs->material];
		liren_context_set_material (context, material);
		liren_context_set_material_shader (context, material);
		liren_context_set_textures (context, material->textures.array, material->textures.count);
		liren_context_bind (context);

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
				blend = LIMAT_ABS (limat_vector_dot (dir, bby));
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

#ifdef LIREN_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

void
liren_draw_opaque (LIRenContext* context,
                   LIRenObject*  object,
                   void*         data)
{
	int i;
	int flags;
	LIRenMaterial* material;
	LIRenModel* model;

	model = object->model;

	/* Rendering mode. */
	flags = LIREN_FLAG_LIGHTING | LIREN_FLAG_TEXTURING;
	liren_context_set_buffer (context, model->buffer);
	liren_context_set_flags (context, flags);
	liren_context_set_modelmatrix (context, &object->orientation.matrix);

	/* Render the mesh. */
	for (i = 0 ; i < model->groups.count ; i++)
	{
		material = model->materials.array[i];
		if (((context->deferred && material->shader_deferred != NULL) ||
		    (!context->deferred && material->shader_forward != NULL)) &&
		    !(material->flags & LIREN_MATERIAL_FLAG_TRANSPARENCY))
		{
			liren_context_set_material (context, material);
			liren_context_set_material_shader (context, material);
			liren_context_set_textures (context, material->textures.array, material->textures.count);
			liren_context_bind (context);
			liren_context_render_indexed (context, model->groups.array[i].start, model->groups.array[i].count);
		}
	}

#ifdef LIREN_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

void
liren_draw_picking (LIRenContext* context,
                    LIRenObject*  object,
                    void*         data)
{
	int i;
	int flags;
	LIRenMaterial* material;
	LIRenModel* model;

	model = object->model;

	/* Rendering mode. */
	flags = LIREN_FLAG_FIXED;
	glLoadName (object->id);
	liren_context_set_flags (context, flags);
	liren_context_set_modelmatrix (context, &object->orientation.matrix);
	liren_context_set_buffer (context, model->buffer);

	/* Render the mesh. */
	for (i = 0 ; i < model->groups.count ; i++)
	{
		material = model->materials.array[i];
		liren_context_bind (context);
		liren_context_render_indexed (context, model->groups.array[i].start, model->groups.array[i].count);
	}
}

void
liren_draw_shadeless (LIRenContext* context,
                      LIRenObject*  object,
                      void*         data)
{
	int i;
	int flags;
	LIRenMaterial* material;
	LIRenModel* model;

	model = object->model;

	/* Rendering mode. */
	flags = LIREN_FLAG_TEXTURING;
	liren_context_set_flags (context, flags);
	liren_context_set_modelmatrix (context, &object->orientation.matrix);
	liren_context_set_buffer (context, model->buffer);

	/* Render the mesh. */
	for (i = 0 ; i < model->groups.count ; i++)
	{
		material = model->materials.array[i];
		liren_context_set_material (context, material);
		liren_context_set_material_shader (context, material);
		liren_context_set_textures (context, material->textures.array, material->textures.count);
		liren_context_bind (context);
		liren_context_render_indexed (context, model->groups.array[i].start, model->groups.array[i].count);
	}

#ifdef LIREN_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

void
liren_draw_shadowmap (LIRenContext* context,
                      LIRenObject*  object,
                      void*         data)
{
	int i;
	LIRenModel* model;

	model = object->model;
	liren_context_set_modelmatrix (context, &object->orientation.matrix);
	liren_context_set_buffer (context, model->buffer);
	liren_context_bind (context);

	/* Render the mesh. */
	for (i = 0 ; i < model->groups.count ; i++)
		liren_context_render_indexed (context, model->groups.array[i].start, model->groups.array[i].count);

#ifdef LIREN_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

void
liren_draw_transparent (LIRenContext* context,
                        LIRenObject*  object,
                        void*         data)
{
	int i;
	int flags;
	LIRenMaterial* material;
	LIRenModel* model;

	model = object->model;

	/* Rendering mode. */
	flags = LIREN_FLAG_LIGHTING | LIREN_FLAG_TEXTURING;
	liren_context_set_flags (context, flags);
	liren_context_set_modelmatrix (context, &object->orientation.matrix);
	liren_context_set_buffer (context, model->buffer);

	/* Render the mesh. */
	for (i = 0 ; i < model->groups.count ; i++)
	{
		material = model->materials.array[i];
		if (((context->deferred && material->shader_deferred != NULL) ||
		    (!context->deferred && material->shader_forward != NULL)) &&
		    (material->flags & LIREN_MATERIAL_FLAG_TRANSPARENCY))
		{
			liren_context_set_material (context, material);
			liren_context_set_material_shader (context, material);
			liren_context_set_textures (context, material->textures.array, material->textures.count);
			liren_context_bind (context);
			liren_context_render_indexed (context, model->groups.array[i].start, model->groups.array[i].count);
		}
	}

#ifdef LIREN_ENABLE_PROFILING
	context->render->profiling.objects++;
#endif
}

/** @} */
/** @} */

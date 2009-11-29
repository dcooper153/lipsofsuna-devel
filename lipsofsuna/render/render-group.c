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
 * \addtogroup lirndObject Object
 * @{
 */

#include <network/lips-network.h>
#include <system/lips-system.h>
#include "render-group.h"
#include "render-scene.h"

static void
private_build_lights (lirndGroup*       self,
                      lirndGroupObject* object);

/*****************************************************************************/

/**
 * \brief Creates a new render group and adds it to the scene.
 *
 * \param scene Scene.
 * \return New group or NULL.
 */
lirndGroup*
lirnd_group_new (lirndScene* scene)
{
	lirndGroup* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirndGroup));
	if (self == NULL)
		return NULL;
	self->scene = scene;
	limat_aabb_init (&self->aabb);

	/* Add to renderer. */
	if (!lialg_ptrdic_insert (scene->groups, self, self))
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the render group.
 *
 * \param self Group.
 */
void
lirnd_group_free (lirndGroup* self)
{
	int i;
	lirndGroupObject* object;
	lirndGroupObject* next;

	for (object = self->objects ; object != NULL ; object = next)
	{
		next = object->next;
		lisys_free (object);
	}
	for (i = 0 ; i < self->lights.count ; i++)
	{
		if (self->realized)
			lirnd_lighting_remove_light (self->scene->lighting, self->lights.array[i]);
		lirnd_light_free (self->lights.array[i]);
	}
	lialg_ptrdic_remove (self->scene->groups, self);
	lisys_free (self->lights.array);
	lisys_free (self);
}

/**
 * \brief Removes all models from the group.
 *
 * \param self Group.
 */
void
lirnd_group_clear (lirndGroup* self)
{
	int i;
	lirndGroupObject* object;
	lirndGroupObject* next;

	for (object = self->objects ; object != NULL ; object = next)
	{
		next = object->next;
		lisys_free (object);
	}
	for (i = 0 ; i < self->lights.count ; i++)
	{
		if (self->realized)
			lirnd_lighting_remove_light (self->scene->lighting, self->lights.array[i]);
		lirnd_light_free (self->lights.array[i]);
	}
	lisys_free (self->lights.array);
	self->lights.array = NULL;
	self->lights.count = 0;
	limat_aabb_init (&self->aabb);
	self->objects = NULL;
}

/**
 * \brief Advances the timer of the group.
 *
 * \param self Group.
 * \param secs Number of seconds since the last update.
 */
void
lirnd_group_update (lirndGroup* self,
                    float       secs)
{
	/* FIXME: We don't need this currently but someone else might. */
}

/**
 * \brief Gets the bounding box of the group.
 *
 * \param self Group.
 * \param result Return location for the bounding box.
 */
void
lirnd_group_get_bounds (const lirndGroup* self,
                        limatAabb*        result)
{
	*result = self->aabb;
}

/**
 * \brief Inserts a model to the group.
 *
 * \param self Group.
 * \param model Model.
 * \param transform Model transformation.
 * \return Nonzero on success.
 */
int
lirnd_group_insert_model (lirndGroup*     self,
                          lirndModel*     model,
                          limatTransform* transform)
{
	limatAabb aabb;
	limatMatrix matrix;
	lirndGroupObject* object;

	/* Allocate object. */
	object = lisys_calloc (1, sizeof (lirndGroupObject));
	if (object == NULL)
		return 0;

	/* Calculate bounds. */
	matrix = limat_convert_transform_to_matrix (*transform);
	lirnd_model_get_bounds (model, &aabb);
	aabb = limat_aabb_transform (aabb, &matrix);
	if (self->objects != NULL)
		self->aabb = limat_aabb_union (self->aabb, aabb);
	else
		self->aabb = aabb;

	/* Add to list. */
	object->model = model;
	object->transform = *transform;
	object->aabb = aabb;
	object->next = self->objects;
	self->objects = object;
	private_build_lights (self, object);

	return 1;
}

/**
 * \brief Checks if the group is renderable.
 *
 * Returns nonzero if the realized flag of the group is set.
 *
 * \param self Group.
 * \return Nonzero if realized.
 */
int
lirnd_group_get_realized (const lirndGroup* self)
{
	return self->realized && self->objects != NULL;
}

/**
 * \brief Sets the realized flag of the group.
 *
 * \param self Group.
 * \param value Flag value.
 * \return Nonzero if succeeded.
 */
int
lirnd_group_set_realized (lirndGroup* self,
                          int         value)
{
	int i;

	/* Register or unregister light sources. */
	if (!self->realized && value)
	{
		for (i = 0 ; i < self->lights.count ; i++)
			lirnd_lighting_insert_light (self->scene->lighting, self->lights.array[i]);
	}
	else if (self->realized && !value)
	{
		for (i = 0 ; i < self->lights.count ; i++)
			lirnd_lighting_remove_light (self->scene->lighting, self->lights.array[i]);
	}

	/* Set state. */
	self->realized = value;

	return 1;
}

/*****************************************************************************/

static void
private_build_lights (lirndGroup*       self,
                      lirndGroupObject* object)
{
	limatTransform tmp;
	limdlNode* node;
	limdlNodeIter iter;
	lirndLight* light;

	LIMDL_FOREACH_NODE (iter, &object->model->model->nodes)
	{
		node = iter.value;
		if (node->type != LIMDL_NODE_LIGHT)
			continue;
		light = lirnd_light_new_from_model (self->scene, node);
		if (light == NULL)
			continue;
		if (!lialg_array_append (&self->lights, &light))
		{
			lirnd_light_free (light);
			continue;
		}
		tmp = light->transform;
		tmp = limat_transform_multiply (object->transform, tmp);
		lirnd_light_set_transform (light, &tmp);
		if (self->realized)
			lirnd_lighting_insert_light (self->scene->lighting, light);
	}
}

/** @} */
/** @} */

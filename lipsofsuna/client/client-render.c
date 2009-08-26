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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliRender Render
 * @{
 */

#include "client.h"

#define LICLI_OBJECT_POSITION_SMOOTHING 0.5f
#define LICLI_OBJECT_ROTATION_SMOOTHING 0.5f

static int
private_engine_free (licliModule* self,
                     liengEngine* engine)
{
	if (self->scene != NULL)
		lirnd_scene_free (self->scene);
	if (self->render != NULL)
		lirnd_render_free (self->render);

	return 1;
}

static int
private_model_new (licliModule* self,
                   liengModel*  model)
{
	lirnd_resources_insert_model (self->render->resources, model->name, model->model);

	return 1;
}

static int
private_model_free (licliModule* self,
                    liengModel*  model)
{
	lirnd_resources_remove_model (self->render->resources, model->name);

	return 1;
}

static int
private_object_new (licliModule* self,
                    liengObject* object)
{
	lirnd_object_new (self->scene, object->id);

	return 1;
}

static int
private_object_free (licliModule* self,
                     liengObject* object)
{
	lirndObject* object_;

	object_ = lirnd_scene_find_object (self->scene, object->id);
	if (object_ != NULL)
		lirnd_object_free (object_);

	return 1;
}

static int
private_object_model (licliModule* self,
                      liengObject* object,
                      liengModel*  model)
{
	lirndObject* object_;
	lirndModel* model_;

	object_ = lirnd_scene_find_object (self->scene, object->id);
	if (object_ != NULL)
	{
		if (model != NULL)
		{
			model_ = lirnd_render_find_model (self->render, model->name);
			if (model_ != NULL)
			{
				lirnd_object_set_model (object_, model_, object->pose);
				return 1;
			}
		}
		lirnd_object_set_model (object_, NULL, NULL);
	}

	return 1;
}

static int
private_object_realize (licliModule* self,
                        liengObject* object,
                        int          value)
{
	lirndObject* object_;

	object_ = lirnd_scene_find_object (self->scene, object->id);
	if (object_ != NULL)
		lirnd_object_set_realized (object_, value);

	return 1;
}

static int
private_object_transform (licliModule*    self,
                          liengObject*    object,
                          limatTransform* value)
{
	lirndObject* object_;

	object_ = lirnd_scene_find_object (self->scene, object->id);
	if (object_ != NULL && !lieng_object_get_realized (object))
		lirnd_object_set_transform (object_, value);

	return 1;
}

static int
private_tick (licliModule* self,
              float        secs)
{
	lialgU32dicIter iter;
	liengObject* eobject;
	limatTransform transform;
	limatTransform transform0;
	limatTransform transform1;
	lirndObject* robject;

	LI_FOREACH_U32DIC (iter, self->engine->objects)
	{
		eobject = iter.value;
		robject = lirnd_scene_find_object (self->scene, eobject->id);
		if (robject == NULL)
			continue;

		/* Deform. */
		lirnd_object_deform (robject, eobject->pose);

		/* Interpolate. */
		lirnd_object_get_transform (robject, &transform0);
		lieng_object_get_transform (eobject, &transform1);
		transform0.rotation = limat_quaternion_get_nearest (transform0.rotation, transform1.rotation);
		transform.position = limat_vector_lerp (
			transform1.position, transform0.position,
			0.5f * LICLI_OBJECT_POSITION_SMOOTHING);
		transform.rotation = limat_quaternion_nlerp (
			transform1.rotation, transform0.rotation,
			0.5f * LICLI_OBJECT_ROTATION_SMOOTHING);
		lirnd_object_set_transform (robject, &transform);
	}
	lirnd_render_update (self->render, secs);
	lirnd_scene_update (self->scene, secs);

	return 1;
}

/*****************************************************************************/

int
licli_render_init (licliModule* self)
{
	lieng_engine_insert_call (self->engine, LIENG_CALLBACK_FREE, 1, private_engine_free, self, NULL);
	lieng_engine_insert_call (self->engine, LIENG_CALLBACK_MODEL_NEW, 1, private_model_new, self, NULL);
	lieng_engine_insert_call (self->engine, LIENG_CALLBACK_MODEL_FREE, 1, private_model_free, self, NULL);
	lieng_engine_insert_call (self->engine, LIENG_CALLBACK_OBJECT_NEW, 1, private_object_new, self, NULL);
	lieng_engine_insert_call (self->engine, LIENG_CALLBACK_OBJECT_FREE, 1, private_object_free, self, NULL);
	lieng_engine_insert_call (self->engine, LIENG_CALLBACK_OBJECT_MODEL, 1, private_object_model, self, NULL);
	lieng_engine_insert_call (self->engine, LIENG_CALLBACK_OBJECT_VISIBILITY, 1, private_object_realize, self, NULL);
	lieng_engine_insert_call (self->engine, LIENG_CALLBACK_OBJECT_TRANSFORM, 1, private_object_transform, self, NULL);
	lieng_engine_insert_call (self->engine, LICLI_CALLBACK_TICK, 1, private_tick, self, NULL);
	return 1;
}

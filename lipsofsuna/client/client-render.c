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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliRender Render
 * @{
 */

#include "client.h"

static int
private_engine_free (LICliClient* self,
                     LIEngEngine* engine)
{
	if (self->scene != NULL)
		liren_scene_free (self->scene);
	if (self->render != NULL)
		liren_render_free (self->render);

	return 1;
}

static int
private_object_new (LICliClient* self,
                    LIEngObject* object)
{
	liren_object_new (self->scene, object->id);

	return 1;
}

static int
private_object_free (LICliClient* self,
                     LIEngObject* object)
{
	LIRenObject* object_;

	object_ = liren_scene_find_object (self->scene, object->id);
	if (object_ != NULL)
		liren_object_free (object_);

	return 1;
}

static int
private_object_model (LICliClient* self,
                      LIEngObject* object,
                      LIEngModel*  model)
{
	LIRenObject* object_;
	LIRenModel* model_;

	object_ = liren_scene_find_object (self->scene, object->id);
	if (object_ != NULL)
	{
		if (model != NULL)
		{
			model_ = liren_render_find_model (self->render, model->name);
			if (model_ == NULL)
			{
				liren_render_load_model (self->render, model->name, model->model);
				model_ = liren_render_find_model (self->render, model->name);
			}
			if (model_ != NULL)
			{
				liren_object_set_pose (object_, object->pose);
				liren_object_set_model (object_, model_);
				return 1;
			}
		}
		liren_object_set_pose (object_, NULL);
		liren_object_set_model (object_, NULL);
	}

	return 1;
}

static int
private_object_realize (LICliClient* self,
                        LIEngObject* object,
                        int          value)
{
	LIRenObject* object_;

	object_ = liren_scene_find_object (self->scene, object->id);
	if (object_ != NULL)
		liren_object_set_realized (object_, value);

	return 1;
}

static int
private_object_transform (LICliClient*    self,
                          LIEngObject*    object,
                          LIMatTransform* value)
{
	LIRenObject* object_;

	object_ = liren_scene_find_object (self->scene, object->id);
	if (object_ != NULL)
		liren_object_set_transform (object_, value);

	return 1;
}

static int
private_engine_tick (LICliClient* self,
                     float        secs)
{
	liren_render_update (self->render, secs);
	liren_scene_update (self->scene, secs);

	return 1;
}

/*****************************************************************************/

int
licli_render_init (LICliClient* self)
{
	lical_callbacks_insert (self->callbacks, self->engine, "engine-free", 1, private_engine_free, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "tick", 1, private_engine_tick, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "object-new", 1, private_object_new, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "object-free", 1, private_object_free, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "object-model", 1, private_object_model, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "object-visibility", 1, private_object_realize, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "object-transform", 1, private_object_transform, self, NULL);
	return 1;
}

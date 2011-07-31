/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#include "lipsofsuna/extension.h"
#include "ext-vision-listener.h"

LIExtVisionListener* liext_vision_listener_new (
	LIExtModule* module)
{
	LIExtVisionListener* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtVisionListener));
	if (self == NULL)
		return NULL;
	self->cone_angle = 0.0f;
	self->cone_cosine = 0.5f * M_PI;
	self->cone_factor = 0.5f;
	self->keep_threshold = 5.0f;
	self->scan_radius = 32.0f;
	self->direction = limat_vector_init (0.0f, 0.0f, -1.0f);
	self->module = module;

	/* Allocate the object dictionary. */
	self->objects = lialg_u32dic_new (self->objects);
	if (self->objects == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void liext_vision_listener_free (
	LIExtVisionListener* self)
{
	lialg_u32dic_free (self->objects);
	lisys_free (self);
}

void liext_vision_listener_clear (
	LIExtVisionListener* self)
{
	lialg_u32dic_clear (self->objects);
}

/**
 * \brief Updates the vision and added the events to a table in the Lua stack.
 *
 * This function is intended to be called only from the scripting API. The
 * table where the events are added is already expected to be at the top of
 * the stack.
 *
 * \param self Vision listener.
 * \param lua Lua state.
 */
void liext_vision_listener_update (
	LIExtVisionListener* self,
	lua_State*           lua)
{
	float dist2;
	float dot;
	float mult;
	float radius2_add;
	float radius2_del;
	float radius2_add_obj;
	float radius2_del_obj;
	LIAlgU32dicIter iter;
	LIEngObject* object;
	LIMatVector diff;
	LIMatVector pos;

	/* Calculate the second powers of the vision radii. */
	radius2_add = self->scan_radius * self->scan_radius;
	radius2_del = self->scan_radius + self->keep_threshold;
	radius2_del *= radius2_del;

	/* Add and remove vision objects. */
	LIALG_U32DIC_FOREACH (iter, self->module->program->engine->objects)
	{
		object = iter.value;

		/* Make sure the object is realized. */
		if (!lieng_object_get_realized (object))
		{
			if (lialg_u32dic_find (self->objects, object->id) != NULL)
			{
				lialg_u32dic_remove (self->objects, object->id);
				lua_pushnumber (lua, lua_objlen (lua, -1) + 1);
				lua_newtable (lua);
				if (liscr_pushdata (lua, object->script))
				{
					lua_setfield (lua, -2, "object");
					lua_pushstring (lua, "object-hidden");
					lua_setfield (lua, -2, "type");
					lua_settable (lua, -3);
				}
				else
					lua_pop (lua, 2);
			}
			continue;
		}

		/* Calculate the distance to the object. */
		pos = object->transform.position;
		diff = limat_vector_subtract (pos, self->position);
		dist2 = limat_vector_dot (diff, diff);
		dot = limat_vector_dot (self->direction, limat_vector_normalize (diff));
		radius2_add_obj = radius2_add;
		radius2_del_obj = radius2_del;
		if (dot < self->cone_cosine)
		{
			mult = (dot + 1.0f) / (self->cone_cosine + 1.0f);
			mult = (1.0f - mult) * self->cone_factor + mult;
			radius2_add_obj *= mult;
			radius2_del_obj *= mult;
		}

		/* Check if the object just entered the vision sphere. */
		if (lialg_u32dic_find (self->objects, object->id) == NULL)
		{
			if (dist2 <= radius2_add_obj)
			{
				lialg_u32dic_insert (self->objects, object->id, NULL + 1);
				lua_pushnumber (lua, lua_objlen (lua, -1) + 1);
				lua_newtable (lua);
				if (liscr_pushdata (lua, object->script))
				{
					lua_setfield (lua, -2, "object");
					lua_pushstring (lua, "object-shown");
					lua_setfield (lua, -2, "type");
					lua_settable (lua, -3);
				}
				else
					lua_pop (lua, 2);
			}
		}

		/* Check if the object has just left the vision sphere. */
		else
		{
			if (dist2 > radius2_del_obj)
			{
				lialg_u32dic_remove (self->objects, object->id);
				lua_pushnumber (lua, lua_objlen (lua, -1) + 1);
				lua_newtable (lua);
				if (liscr_pushdata (lua, object->script))
				{
					lua_setfield (lua, -2, "object");
					lua_pushstring (lua, "object-hidden");
					lua_setfield (lua, -2, "type");
					lua_settable (lua, -3);
				}
				else
					lua_pop (lua, 2);
			}
		}
	}

	/* TODO: Silently remove garbage collected objects. */
}

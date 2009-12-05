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
 * \addtogroup liextcliEffect Effect
 * @{
 */

#include <client/lips-client.h>
#include <script/lips-script.h>
#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Effect"
 * ---
 * -- Create special effects.
 * -- @name Effect
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new particle.
 * --
 * -- @param self Effect class.
 * -- @param pos Position vector.
 * -- @param vel Velocity vector.
 * function Effect.particle(self, pos, vel)
 */
static int
Effect_particle (lua_State* lua)
{
	liextModule* module;
	liparPoint* point;
	liscrData* pos;
	liscrData* vel;
	liscrScript* script;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EFFECT);
	pos = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	vel = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);

	point = lirnd_scene_insert_particle (module->module->scene, pos->data, vel->data);
	if (point != NULL)
	{
		point->time_fade = 0.0f;
		point->time_life = 1.0f;
	}
	return 0;
}

/* @luadoc
 * ---
 * -- Creates random particles.
 * --
 * -- The following values are recognized in the argument table:
 * --
 * -- angle: Maximum cone angle in degrees.
 * -- axis: Axis of rotation.
 * -- accel: Acceleration vector.
 * -- color: Particle color.
 * -- count: Number of particles.
 * -- fade: Fade start time in seconds.
 * -- life: Particle lifetime in seconds.
 * -- random: Velocity error value.
 * -- spread: Maximum particle distance from the center.
 * -- velocity: Average particle velocity.
 * --
 * -- @param self Effect class.
 * -- @param pos Position vector.
 * -- @param num Number of particles.
 * -- @param args Optional table of arguments.
 * function Effect.random(self, args)
 */
static int
Effect_random (lua_State* lua)
{
	int i;
	int count;
	float angle;
	float color[3];
	float fade;
	float life;
	float random;
	float spread;
	float velocity;
	liextModule* module;
	limatQuaternion rot0;
	limatQuaternion rot1;
	limatVector tmp;
	limatVector axis;
	limatVector accel;
	limatVector position;
	limatVector partpos;
	limatVector partvel;
	liparPoint* particle;
	liscrData* vector;

	/* Mandatory arguments. */
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EFFECT);
	vector = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	position = *((limatVector*) vector->data);

	/* Default values. */
	axis = limat_vector_init (0.0f, 1.0f, 0.0f);
	accel = limat_vector_init (0.0f, -9.8f, 0.0f);
	angle = M_PI;
	color[0] = 1.0f;
	color[1] = 1.0f;
	color[2] = 1.0f;
	count = 10;
	fade = 1.0f;
	life = 2.0f;
	random = 0.0f;
	spread = 0.0f;
	velocity = 10.0f;

	/* Optional arguments. */
	if (lua_type (lua, 3) == LUA_TTABLE)
	{
		/* "angle" */
		lua_getfield (lua, 3, "angle");
		if (lua_isnumber (lua, -1))
			angle = lua_tonumber (lua, -1) / 180.0f * M_PI;
		lua_pop (lua, 1);

		/* "axis" */
		lua_getfield (lua, 3, "axis");
		vector = liscr_isdata (lua, -1, LICOM_SCRIPT_VECTOR);
		if (vector != NULL)
		{
			axis = *((limatVector*) vector->data);
			axis = limat_vector_normalize (axis);
		}
		lua_pop (lua, 1);

		/* "accel" */
		lua_getfield (lua, 3, "accel");
		vector = liscr_isdata (lua, -1, LICOM_SCRIPT_VECTOR);
		if (vector != NULL)
			accel = *((limatVector*) vector->data);
		lua_pop (lua, 1);

		/* "color" */
		lua_getfield (lua, 3, "color");
		if (lua_type (lua, -1) == LUA_TTABLE)
		{
			lua_pushnumber (lua, 1);
			lua_gettable (lua, -2);
			if (lua_isnumber (lua, -1))
				color[0] = lua_tonumber (lua, -1);
			lua_pop (lua, 1);

			lua_pushnumber (lua, 2);
			lua_gettable (lua, -2);
			if (lua_isnumber (lua, -1))
				color[1] = lua_tonumber (lua, -1);
			lua_pop (lua, 1);

			lua_pushnumber (lua, 3);
			lua_gettable (lua, -2);
			if (lua_isnumber (lua, -1))
				color[2] = lua_tonumber (lua, -1);
			lua_pop (lua, 1);
		}
		lua_pop (lua, 1);

		/* "count" */
		lua_getfield (lua, 3, "count");
		if (lua_isnumber (lua, -1))
			count = lua_tointeger (lua, -1);
		lua_pop (lua, 1);

		/* "life" */
		lua_getfield (lua, 3, "life");
		if (lua_isnumber (lua, -1))
		{
			life = lua_tonumber (lua, -1);
			fade = 0.5f * life;
		}
		lua_pop (lua, 1);

		/* "fade" */
		lua_getfield (lua, 3, "fade");
		if (lua_isnumber (lua, -1))
			fade = lua_tonumber (lua, -1);
		lua_pop (lua, 1);

		/* "random" */
		lua_getfield (lua, 3, "random");
		if (lua_isnumber (lua, -1))
			random = lua_tonumber (lua, -1);
		lua_pop (lua, 1);

		/* "velocity" */
		lua_getfield (lua, 3, "velocity");
		if (lua_isnumber (lua, -1))
			velocity = lua_tonumber (lua, -1);
		lua_pop (lua, 1);
	}

	/* Create particles. */
	for (i = 0 ; i < count ; i++)
	{
		tmp = limat_vector_cross (axis, limat_vector_init (1.0f, 0.0f, 0.0f));
		if (limat_vector_get_length (tmp) < 0.1f)
			tmp = limat_vector_cross (axis, limat_vector_init (0.0f, 1.0f, 0.0f));
		rot0 = limat_quaternion_rotation ((2.0f * rand () / RAND_MAX - 1.0f) * angle, tmp);
		rot1 = limat_quaternion_rotation ((2.0f * rand () / RAND_MAX - 1.0f) * M_PI, axis);
		tmp = limat_quaternion_transform (rot0, axis);
		tmp = limat_quaternion_transform (rot1, tmp);
		partpos = limat_vector_multiply (tmp, spread * (1.0f * rand () / RAND_MAX));
		partpos = limat_vector_add (partpos, position);
		partvel = limat_vector_multiply (tmp, velocity + velocity * random *
			(2.0f * rand () / RAND_MAX - 1.0f));
		particle = lirnd_scene_insert_particle (module->module->scene, &partpos, &partvel);
		if (particle != NULL)
		{
			particle->color[0] = color[0];
			particle->color[1] = color[1];
			particle->color[2] = color[2];
			particle->time_fade = fade;
			particle->time_life = life;
			particle->acceleration = accel;
		}
	}

	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new ray effect.
 * --
 * -- @param self Effect class.
 * -- @param src Source position vector.
 * -- @param dst Destination position vector.
 * -- @param life Ray life time.
 * function Effect.ray(self, src, dst, life)
 */
static int
Effect_ray (lua_State* lua)
{
	float life;
	liextModule* module;
	limatVector tmp;
	liparLine* line;
	liscrData* src;
	liscrData* dst;
	liscrScript* script;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EFFECT);
	src = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	dst = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);
	life = luaL_checknumber (lua, 4);

	/* FIXME */
	tmp = limat_vector_init (0.0f, 0.0f, 0.0f);
	line = lipar_manager_insert_line (module->module->scene->particles, src->data, dst->data, &tmp, &tmp);
	if (line != NULL)
	{
		line->time_fade[0] = line->time_fade[1] = 0.0f;
		line->time_life[0] = line->time_life[1] = life;
	}
	return 0;
}

/* @luadoc
 * ---
 * -- Creates a particle system.
 * --
 * -- @param self Effect class.
 * -- @param pos Position vector.
 * -- @param name Effect name.
 * function Effect.system(self, pos, name)
 */
static int
Effect_system (lua_State* lua)
{
	int i;
	const char* effect;
	liextModule* module;
	limatVector position;
	limatVector velocity;
	liparPoint* particle;
	liscrData* vector;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EFFECT);
	vector = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	effect = luaL_checkstring (lua, 3);

#warning Effect_system is not implemented properly yet.
	for (i = 0 ; i < 100 ; i++)
	{
		position = *((limatVector*) vector->data);;
		velocity = limat_vector_init (
			0.1*(rand()/(0.5*RAND_MAX)-1.0),
			0.1*(rand()/(3.0*RAND_MAX)+3.0),
			0.1*(rand()/(0.5*RAND_MAX)-1.0));
		particle = lirnd_scene_insert_particle (module->module->scene, &position, &velocity);
		if (particle != NULL)
		{
			particle->time_life = 2.0f;
			particle->time_fade = 1.0f;
			particle->acceleration = limat_vector_init (0.0, -10.0, 0.5);
		}
	}

	return 0;
}

/*****************************************************************************/

void
liextEffectScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_EFFECT, data);
	liscr_class_insert_func (self, "particle", Effect_particle);
	liscr_class_insert_func (self, "random", Effect_random);
	liscr_class_insert_func (self, "ray", Effect_ray);
	liscr_class_insert_func (self, "system", Effect_system);
}

/** @} */
/** @} */
/** @} */

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
 * \addtogroup liextcliEffect Effect
 * @{
 */

#include <lipsofsuna/client.h>
#include <lipsofsuna/script.h>
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
 * -- Arguments:
 * -- accel: Acceleration vector.
 * -- color: Particle color.
 * -- fade: Fade start time in seconds.
 * -- life: Particle lifetime in seconds.
 * -- position: Position vector. (required)
 * -- velocity: Average particle velocity.
 * --
 * -- @param self Effect class.
 * -- @param args Arguments.
 * function Effect.particle(self, args)
 */
static void Effect_particle (LIScrArgs* args)
{
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float fade = 0.5f;
	float life = 1.0f;
	LIExtModule* module;
	LIMatVector position;
	LIMatVector accel = { 0.0f, -9.8f, 0.0f };
	LIMatVector velocity = { 0.0f, 0.0f, 0.0f };
	LIParPoint* particle;

	/* Mandatory arguments. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_EFFECT);
	if (!liscr_args_gets_vector (args, "position", &position))
		return;

	/* Optional arguments. */
	liscr_args_gets_vector (args, "accel", &accel);
	liscr_args_gets_vector (args, "velocity", &velocity);
	liscr_args_gets_floatv (args, "color", 4, color);
	if (liscr_args_gets_float (args, "life", &life))
		fade = 0.5f * life;
	liscr_args_gets_float (args, "fade", &fade);

	/* Create particle. */
	particle = liren_scene_insert_particle (module->client->scene, &position, &velocity);
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

/* @luadoc
 * ---
 * -- Creates random particles.
 * --
 * -- The following values are recognized in the argument table:
 * --
 * -- Arguments:
 * -- accel: Acceleration vector.
 * -- angle: Maximum cone angle in degrees.
 * -- axis: Axis of rotation.
 * -- color: Particle color.
 * -- count: Number of particles.
 * -- fade: Fade start time in seconds.
 * -- life: Particle lifetime in seconds.
 * -- position: Position vector. (required)
 * -- random: Velocity error value.
 * -- spread: Maximum particle distance from the center.
 * -- velocity: Average particle velocity.
 * --
 * -- @param self Effect class.
 * -- @param args Arguments.
 * function Effect.random(self, args)
 */
static void Effect_random (LIScrArgs* args)
{
	int i;
	int count;
	float angle;
	float color[4];
	float fade;
	float life;
	float random;
	float spread;
	float velocity;
	LIExtModule* module;
	LIMatQuaternion rot0;
	LIMatQuaternion rot1;
	LIMatVector tmp;
	LIMatVector axis;
	LIMatVector accel;
	LIMatVector position;
	LIMatVector partpos;
	LIMatVector partvel;
	LIParPoint* particle;

	/* Mandatory arguments. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_EFFECT);
	if (!liscr_args_gets_vector (args, "position", &position))
		return;

	/* Default values. */
	axis = limat_vector_init (0.0f, 1.0f, 0.0f);
	accel = limat_vector_init (0.0f, -9.8f, 0.0f);
	angle = M_PI;
	color[0] = 1.0f;
	color[1] = 1.0f;
	color[2] = 1.0f;
	color[3] = 1.0f;
	count = 10;
	fade = 1.0f;
	life = 2.0f;
	random = 0.0f;
	spread = 0.0f;
	velocity = 10.0f;

	/* Optional arguments. */
	liscr_args_gets_vector (args, "accel", &accel);
	if (liscr_args_gets_float (args, "angle", &angle))
		angle = angle / 180.0f * M_PI;
	if (liscr_args_gets_vector (args, "axis", &axis))
		axis = limat_vector_normalize (axis);
	liscr_args_gets_floatv (args, "color", 4, color);
	liscr_args_gets_int (args, "count", &count);
	if (liscr_args_gets_float (args, "life", &life))
		fade = 0.5f * life;
	liscr_args_gets_float (args, "fade", &fade);
	liscr_args_gets_float (args, "random", &random);
	liscr_args_gets_float (args, "velocity", &velocity);

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
		particle = liren_scene_insert_particle (module->client->scene, &partpos, &partvel);
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
}

/* @luadoc
 * ---
 * -- Creates a new ray effect.
 * --
 * -- dst: Destination position vector. (required)
 * -- fade: Fade start time in seconds.
 * -- life: Particle lifetime in seconds.
 * -- src: Source position vector. (required)
 * --
 * -- @param self Effect class.
 * -- @param args Arguments.
 * function Effect.ray(self, args)
 */
static void Effect_ray (LIScrArgs* args)
{
	float fade = 0.5f;
	float life = 1.0f;
	LIExtModule* module;
	LIMatVector dst;
	LIMatVector src;
	LIMatVector tmp;
	LIParLine* line;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_EFFECT);
	if (!liscr_args_gets_vector (args, "dst", &dst) ||
	    !liscr_args_gets_vector (args, "src", &src) )
		return;
	if (liscr_args_gets_float (args, "life", &life))
		fade = 0.5f * life;
	liscr_args_gets_float (args, "fade", &fade);

	/* FIXME */
	tmp = limat_vector_init (0.0f, 0.0f, 0.0f);
	line = lipar_manager_insert_line (module->client->scene->particles, &src, &dst, &tmp, &tmp);
	if (line != NULL)
	{
		line->time_fade[0] = line->time_fade[1] = fade;
		line->time_life[0] = line->time_life[1] = life;
	}
}

/* @luadoc
 * ---
 * -- Creates a particle system.
 * --
 * -- Arguments:
 * -- position: Position vector.
 * --
 * -- @param self Effect class.
 * -- @param args Arguments.
 * function Effect.system(self, args)
 */
static void Effect_system (LIScrArgs* args)
{
	int i;
	LIExtModule* module;
	LIMatVector position;
	LIMatVector velocity;
	LIParPoint* particle;

	/* Mandatory arguments. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_EFFECT);
	if (!liscr_args_gets_vector (args, "position", &position))
		return;

#warning Effect_system is not implemented properly yet.
	for (i = 0 ; i < 100 ; i++)
	{
		velocity = limat_vector_init (
			0.1*(rand()/(0.5*RAND_MAX)-1.0),
			0.1*(rand()/(3.0*RAND_MAX)+3.0),
			0.1*(rand()/(0.5*RAND_MAX)-1.0));
		particle = liren_scene_insert_particle (module->client->scene, &position, &velocity);
		if (particle != NULL)
		{
			particle->time_life = 2.0f;
			particle->time_fade = 1.0f;
			particle->acceleration = limat_vector_init (0.0, -10.0, 0.5);
		}
	}
}

/*****************************************************************************/

void
liext_script_effect (LIScrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_EFFECT, data);
	liscr_class_insert_cfunc (self, "particle", Effect_particle);
	liscr_class_insert_cfunc (self, "random", Effect_random);
	liscr_class_insert_cfunc (self, "ray", Effect_ray);
	liscr_class_insert_cfunc (self, "system", Effect_system);
}

/** @} */
/** @} */
/** @} */

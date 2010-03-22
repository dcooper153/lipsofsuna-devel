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
 * \addtogroup LIRenLight Light
 * @{
 */

#include <lipsofsuna/system.h>
#include "render-draw.h"
#include "render-light.h"

#define LIGHT_CONTRIBUTION_EPSILON 0.001f

static void
private_update_shadow (LIRenLight* self);

/*****************************************************************************/

/**
 * \brief Creates a new light source.
 *
 * \param scene Scene.
 * \param color Array of 4 floats.
 * \param equation Array of 3 floats.
 * \param cutoff Spot cutoff in radians.
 * \param exponent Spot expoent.
 * \param shadows Nonzero if the lamp casts shadows.
 * \return New light source or NULL.
 */
LIRenLight*
liren_light_new (LIRenScene*  scene,
                 const float* color,
                 const float* equation,
                 float        cutoff,
                 float        exponent,
                 int          shadows)
{
	LIRenLight* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenLight));
	if (self == NULL)
		return NULL;
	self->scene = scene;
	self->ambient[0] = 0.0f;
	self->ambient[1] = 0.0f;
	self->ambient[2] = 0.0f;
	self->ambient[3] = 1.0f;
	self->diffuse[0] = color[0];
	self->diffuse[1] = color[1];
	self->diffuse[2] = color[2];
	self->diffuse[3] = 1.0f;
	self->specular[0] = color[0];
	self->specular[1] = color[1];
	self->specular[2] = color[2];
	self->specular[3] = 1.0f;
	self->equation[0] = equation[0];
	self->equation[1] = equation[1];
	self->equation[2] = equation[2];
	self->cutoff = cutoff;
	self->exponent = exponent;
	self->projection = limat_matrix_identity ();
	self->modelview = limat_matrix_identity ();
	self->modelview_inverse = limat_matrix_identity ();
	self->transform = limat_transform_identity ();
	if (!shadows)
		return self;
	if (!GLEW_EXT_framebuffer_object || !GLEW_ARB_depth_buffer_float)
		return self;

	/* Create shadow texture. */
	glGenTextures (1, &self->shadow.map);
	glBindTexture (GL_TEXTURE_2D, self->shadow.map);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOWMAPSIZE, SHADOWMAPSIZE,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	/* Create framebuffer object. */
	glGenFramebuffersEXT (1, &self->shadow.fbo);
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->shadow.fbo);
	glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT,
		GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, self->shadow.map, 0);
	glDrawBuffer (GL_FALSE);
	glReadBuffer (GL_FALSE);
	switch (glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT))
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			lisys_error_set (ENOTSUP, "cannot create framebuffer object");
			glDeleteFramebuffersEXT (1, &self->shadow.fbo);
			self->shadow.fbo = 0;
			break;
	}
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

	return self;
}

/**
 * \brief Creates a new directional light source.
 *
 * \param scene Scene.
 * \param color Array of 4 floats.
 * \return New light source or NULL.
 */
LIRenLight*
liren_light_new_directional (LIRenScene*  scene,
                             const float* color)
{
	const float equation[3] = { 1.0f, 0.0f, 0.0f };
	const LIMatVector direction = { 0.0f, 0.0f, -1.0f };
	LIRenLight* self;

	/* Create the sun.  */
	self = liren_light_new (scene, color, equation, M_PI, 0.0f, 0);
	if (self == NULL)
		return NULL;
	liren_light_set_direction (self, &direction);

	return self;
}

/**
 * \brief Creates a new light from a model light.
 *
 * \param scene Scene.
 * \param light Model light.
 * \return New light or NULL.
 */
LIRenLight*
liren_light_new_from_model (LIRenScene*      scene,
                            const LIMdlNode* light)
{
	LIMatMatrix projection;
	LIMatTransform transform;
	LIRenLight* self;

	/* Allocate self. */
	self = liren_light_new (scene,
		light->light.color, light->light.equation,
		light->light.spot.cutoff, light->light.spot.exponent,
		light->light.flags & LIMDL_LIGHT_FLAG_SHADOW);
	if (self == NULL)
		return NULL;

	/* Set transform. */
	self->node = light;
	limdl_node_get_world_transform (light, &transform);
	liren_light_set_transform (self, &transform);
	limdl_light_get_projection (light, &projection);
	liren_light_set_projection (self, &projection);

	return self;
}

/**
 * \brief Frees a light source.
 *
 * \param self Light source.
 */
void liren_light_free (LIRenLight* self)
{
	if (GLEW_EXT_framebuffer_object)
		glDeleteFramebuffersEXT (1, &self->shadow.fbo);
	glDeleteTextures (1, &self->shadow.map);
	liren_lighting_remove_light (self->scene->lighting, self);
	lisys_free (self);
}

/**
 * \brief Compares the contributions of two lights.
 *
 * The function inspects the contribution values previously set by the engine and
 * returns and integer that is -1 if the first light contributes more than the
 * second one, 0 if they contribute as much, and 1 if the second light contributes
 * more.
 *
 * \param self Light source.
 * \param light Light source.
 * \return Integer indicating which light contributes more.
 */
int
liren_light_compare (const LIRenLight* self,
                     const LIRenLight* light)
{
	if (self->rating < light->rating)
		return -1;
	if (self->rating > light->rating)
		return 1;
	return 0;
}

void
liren_light_update (LIRenLight* self)
{
	if (self->shadow.map)
		private_update_shadow (self);
}

void
liren_light_set_ambient (LIRenLight*  self,
                         const float* value)
{
	memcpy (self->ambient, value, 4 * sizeof (float));
}

/**
 * \brief Gets the area of effect of the light source.
 *
 * \param self Light source.
 * \param result Return location for a bounding box.
 * \return Nonzero if has bounds.
 */
int
liren_light_get_bounds (const LIRenLight* self,
                        LIMatAabb*        result)
{
	double a;
	double b;
	double c;
	double r;
	double det;
	double eps;

	/* Choose epsilon. */
	eps = LIMAT_MAX (LIMAT_MAX (self->diffuse[0], self->diffuse[1]), self->diffuse[2]);
	eps /= 256.0;
	if (eps < LIGHT_CONTRIBUTION_EPSILON)
		eps = LIGHT_CONTRIBUTION_EPSILON;

	/* Solve radius. */
	/* 1 / (A * r^2 + B * r + C) = E */
	/* (EA) * r^2 + (EB) * r + (Ec-1) = 0 */
	a = eps * self->equation[0];
	b = eps * self->equation[1];
	c = eps * self->equation[2] - 1.0;
	det = b * b - 4 * a * c;
	if (det < 0.0)
		return 0;
	r = (-b + sqrt (det)) / (2.0 * a);
	r = r + 0.5;

	/* Create bounding box. */
	result->min = limat_vector_subtract (self->transform.position, limat_vector_init (r, r, r));
	result->max = limat_vector_add (self->transform.position, limat_vector_init (r, r, r));

	return 1;
}

/**
 * \brief Gets the forwards direction of the light.
 *
 * The direction is derived from the transformation of the light.
 *
 * \param self Light source.
 * \param value Return location for the direction.
 */
void
liren_light_get_direction (const LIRenLight* self,
                           LIMatVector*      value)
{
	*value = limat_vector_init (0.0f, 0.0f, -1.0f);
	*value = limat_quaternion_transform (self->transform.rotation, *value);
}

/**
 * \brief Makes the light directional and sets it direction.
 *
 * \param self Light source.
 * \param value Light direction.
 */
void
liren_light_set_direction (LIRenLight*        self,
                           const LIMatVector* value)
{
	float a;
	float b;
	LIMatMatrix projection;
	LIMatQuaternion rotation;
	LIMatTransform transform;
	LIMatVector direction;
	LIMatVector position;

	/* Calculate temporary position. */
	/* FIXME: Temporary position not supported. */
	direction = *value;
	position = limat_vector_init (0.0f, 0.0f, 0.0f);

	/* Calculate light rotation. */
	a = limat_vector_dot (direction, limat_vector_init (0.0f, 1.0f, 0.0f));
	b = limat_vector_dot (direction, limat_vector_init (0.0f, 0.0f, 1.0f));
	if (LIMAT_ABS (a) >= LIMAT_ABS (b))
		rotation = limat_quaternion_look (direction, limat_vector_init (0.0f, 1.0f, 0.0f));
	else
		rotation = limat_quaternion_look (direction, limat_vector_init (0.0f, 0.0f, 1.0f));

	/* Set light transformation. */
	transform = limat_transform_init (position, rotation);
	liren_light_set_transform (self, &transform);
	projection = limat_matrix_ortho (200, -200, 200, -200, -1000, 1000);
	liren_light_set_projection (self, &projection);
	self->directional = 1;
}

/**
 * \brief Enables or disables directional mode.
 *
 * \param self Light source.
 * \param value Nonzero if the light is directional.
 */
void
liren_light_set_directional (LIRenLight* self,
                             int         value)
{
	self->directional = (value != 0);
}

/**
 * \brief Checks if the light is registered.
 *
 * \param self Light source.
 * \return Nonzero if registered.
 */
int
liren_light_get_enabled (const LIRenLight* self)
{
	return self->enabled;
}

/**
 * \brief Gets the modelview matrix of the light.
 *
 * \param self Light source.
 * \param value Return location for the matrix.
 */
void
liren_light_get_modelview (const LIRenLight* self,
                           LIMatMatrix*         value)
{
	*value = self->modelview;
}

/**
 * \brief Gets the world position of the light.
 *
 * The position is derived from the modelview matrix of the light.
 *
 * \param self Light source.
 * \param value Return location for the position.
 */
void
liren_light_get_position (const LIRenLight* self,
                          LIMatVector*      value)
{
	*value = self->transform.position;
}

/**
 * \brief Gets the projection matrix of the light.
 *
 * \param self Light source.
 * \param value Return location for the matrix.
 */
void
liren_light_get_projection (const LIRenLight* self,
                            LIMatMatrix*         value)
{
	*value = self->projection;
}

/**
 * \brief Sets the projection matrix of the light.
 *
 * \param self Light source.
 * \param value Matrix to set.
 */
void
liren_light_set_projection (LIRenLight*     self,
                            const LIMatMatrix* value)
{
	self->projection = *value;
}

/**
 * \brief Gets the transformation of the light.
 *
 * \param self Light source.
 * \param value Return value for the transformation.
 */
void
liren_light_get_transform (LIRenLight*     self,
                           LIMatTransform* value)
{
	*value = self->transform;
}

/**
 * \brief Sets the transformation of the light.
 *
 * This modifies the modelview matrix.
 *
 * \param self Light source.
 * \param transform Transformation.
 */
void
liren_light_set_transform (LIRenLight*           self,
                           const LIMatTransform* transform)
{
	LIMatVector dir;
	LIMatVector pos;
	LIMatVector up;

	pos = transform->position;
	dir = limat_quaternion_transform (transform->rotation, limat_vector_init (0.0f, 0.0f, -1.0f));
	up = limat_quaternion_transform (transform->rotation, limat_vector_init (0.0f, 1.0f, 0.0f));
	self->transform = *transform;
	self->modelview = limat_matrix_look (pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, up.x, up.y, up.z);
	self->modelview_inverse = limat_matrix_invert (self->modelview);
}

/*****************************************************************************/

static void
private_update_shadow (LIRenLight* self)
{
	LIAlgU32dicIter iter;
	LIMatFrustum frustum;
	LIRenContext* context;
	LIRenShader* shader;

	/* Find shader. */
	shader = liren_render_find_shader (self->scene->render, "shadowmap");
	if (shader == NULL)
		return;

	/* Enable depth rendering mode. */
	glPushAttrib (GL_VIEWPORT_BIT);
	glViewport (0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE);
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->shadow.fbo);
	glClear (GL_DEPTH_BUFFER_BIT);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_CULL_FACE);
	glFrontFace (GL_CCW);
	glDepthFunc (GL_LEQUAL);
	glBindTexture (GL_TEXTURE_2D, 0);

	/* Render to depth texture. */
	limat_frustum_init (&frustum, &self->modelview, &self->projection);
	context = liren_render_get_context (self->scene->render);
	liren_context_set_scene (context, self->scene);
	liren_context_set_modelview (context, &self->modelview);
	liren_context_set_projection (context, &self->projection);
	liren_context_set_frustum (context, &frustum);
	liren_context_set_shader (context, shader);
	LIALG_U32DIC_FOREACH (iter, self->scene->objects)
		liren_draw_shadowmap (context, iter.value, self);

	/* Disable depth rendering mode. */
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	glPopAttrib ();
}

/** @} */
/** @} */

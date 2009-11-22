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
 * \addtogroup lirndRender Render
 * @{
 */

#include <system/lips-system.h>
#include "render.h"
#include "render-draw.h"
#include "render-group.h"

static int
private_init_resources (lirndRender* self,
                        const char*  dir);

static int
private_init_shaders (lirndRender* self);

static liimgTexture*
private_load_image (lirndRender* self,
                    lirndImage*  texture);

/*****************************************************************************/

lirndRender*
lirnd_render_new (const char* dir)
{
	lirndRender* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirndRender));
	if (self == NULL)
		return NULL;
	self->shader.enabled = 0;//(livid_features.shader_model >= 3);

	/* Allocate scene list. */
	self->scenes = lialg_ptrdic_new ();
	if (self->scenes == NULL)
		goto error;

	/* Load data. */
	self->config.dir = listr_dup (dir);
	if (self->config.dir == NULL)
		goto error;
	if (!private_init_shaders (self) ||
	    !private_init_resources (self, dir))
		goto error;

	return self;

error:
	lirnd_render_free (self);
	return NULL;
}

void
lirnd_render_free (lirndRender* self)
{
	/* Free resources. */
	if (self->resources != NULL)
		lirnd_resources_free (self->resources);
	glDeleteTextures (1, &self->helpers.noise);
	glDeleteTextures (1, &self->helpers.depth_texture_max);

	/* Free shaders. */
	if (self->shader.fixed != NULL)
		lirnd_shader_free (self->shader.fixed);
	if (self->shader.shadowmap != NULL)
		lirnd_shader_free (self->shader.shadowmap);

	/* Free scenes. */
	if (self->scenes != NULL)
	{
		assert (self->scenes->size == 0);
		lialg_ptrdic_free (self->scenes);
	}

	lisys_free (self->config.dir);
	lisys_free (self);
}

/**
 * \brief Finds a shader by name.
 *
 * If no matching shader is found, a fixed function fallback is returned.
 *
 * \param self Renderer.
 * \param name Name of the shader.
 * \return Shader.
 */
lirndShader*
lirnd_render_find_shader (lirndRender* self,
                          const char*  name)
{
	lirndShader* shader;

	shader = lirnd_resources_find_shader (self->resources, name);
	if (shader == NULL)
		return self->shader.fixed;

	return shader;
}

/**
 * \brief Finds a texture by name.
 *
 * Searches for a texture from the texture cache and returns the match, if any.
 * If no match is found, the texture is loaded from a file, added to the cache,
 * and returned. NULL is returned if loading the texture fails.
 *
 * \param self Renderer.
 * \param name Name of the texture.
 * \return Texture or NULL.
 */
lirndImage*
lirnd_render_find_image (lirndRender* self,
                         const char*  name)
{
	lirndImage* image;

	image = lirnd_resources_find_image (self->resources, name);
	if (image == NULL)
		image = lirnd_resources_insert_image (self->resources, name);
	if (image == NULL || image->invalid)
		return NULL;
	if (image->texture == NULL)
	{
		image->texture = private_load_image (self, image);
		if (image->texture == NULL)
		{
			image->invalid = 1;
			lisys_error_report ();
		}
	}
	if (image->invalid)
		return NULL;

	return image;
}

/**
 * \brief Finds a model by name.
 *
 * \param self Renderer.
 * \param name Model name.
 * \return Model.
 */
lirndModel*
lirnd_render_find_model (lirndRender* self,
                         const char*  name)
{
	return lirnd_resources_find_model (self->resources, name);
}

/**
 * \brief Forces the renderer to load or reload a texture image.
 *
 * Reloads the requested texture and updates any materials that reference it
 * to point to the new texture. Any other references to the texture become
 * invalid and need to be manually replaced.
 *
 * \param self Renderer.
 * \param name Texture name.
 * \return Nonzero on success.
 */
int
lirnd_render_load_image (lirndRender* self,
                         const char*  name)
{
	lialgPtrdicIter iter0;
	lialgU32dicIter iter1;
	lialgStrdicIter iter2;
	liimgTexture* imgtexture;
	lirndImage* rndimage;
	lirndModel* model;
	lirndObject* object;
	lirndScene* scene;

	/* Find image info. */
	rndimage = lirnd_resources_find_image (self->resources, name);
	if (rndimage == NULL)
		rndimage = lirnd_resources_insert_image (self->resources, name);
	if (rndimage == NULL)
		return 0;
	if (rndimage->texture == NULL)
		return 1;

	/* Reload image. */
	imgtexture = private_load_image (self, rndimage);
	if (imgtexture == NULL)
		return 0;
	if (rndimage->texture != NULL)
		liimg_texture_free (rndimage->texture);
	rndimage->texture = imgtexture;

	/* Replace in all instances. */
	LI_FOREACH_PTRDIC (iter0, self->scenes)
	{
		scene = iter0.value;
		LI_FOREACH_U32DIC (iter1, scene->objects)
		{
			object = iter1.value;
			if (object->instance == NULL)
				continue;
			lirnd_model_replace_image (object->instance, rndimage);
		}
	}

	/* Replace in all models. */
	LI_FOREACH_STRDIC (iter2, self->resources->models)
	{
		model = iter2.value;
		lirnd_model_replace_image (model, rndimage);
	}

	return 1;
}

/**
 * \brief Forces the renderer to load or reload a model.
 *
 * Reloads the requested model and updates any objects or groups that reference
 * it to point to the new model. Any other references to the model become
 * invalid and need to be manually replaced.
 *
 * \param self Renderer.
 * \param name Model name.
 * \param model Model data.
 * \return Nonzero on success.
 */
int
lirnd_render_load_model (lirndRender* self,
                         const char*  name,
                         limdlModel*  model)
{
	lirndGroup* group;
	lirndGroupObject* grpobj;
	lirndObject* object;
	lirndModel* model0;
	lirndModel* model1;
	lirndScene* scene;
	lialgPtrdicIter iter0;
	lialgU32dicIter iter1;
	lialgPtrdicIter iter2;

	/* Create new model. */
	model1 = lirnd_model_new (self, model);
	if (model1 == NULL)
		return 1;

	/* Early exit if not reloading. */
	model0 = lirnd_resources_find_model (self->resources, name);
	if (model0 == NULL)
	{
		if (!lirnd_resources_insert_model (self->resources, name, model1))
		{
			lirnd_model_free (model1);
			return 0;
		}
		return 1;
	}

	/* Replace in all instances. */
	LI_FOREACH_PTRDIC (iter0, self->scenes)
	{
		scene = iter0.value;

		/* Replace in all objects. */
		LI_FOREACH_U32DIC (iter1, scene->objects)
		{
			object = iter1.value;
			if (object->model == model0)
			{
				if (!lirnd_object_set_model (object, model1))
				{
					lirnd_object_set_model (object, NULL);
					lirnd_object_set_pose (object, NULL);
				}
			}
		}

		/* Replace in all groups. */
		LI_FOREACH_PTRDIC (iter2, scene->groups)
		{
			group = iter2.value;
			for (grpobj = group->objects ; grpobj != NULL ; grpobj = grpobj->next)
			{
				if (grpobj->model == model0)
					grpobj->model = model1;
			}
		}
	}

	/* Replace in resource manager. */
	/* FIXME: Leaves the manager to a broken state if fails to allocate memory. */
	lirnd_resources_remove_model (self->resources, name);
	lirnd_resources_insert_model (self->resources, name, model1);

	return 1;
}

/**
 * \brief Updates the renderer state.
 *
 * \param self Renderer.
 * \param secs Number of seconds since the last update.
 */
void
lirnd_render_update (lirndRender* self,
                     float        secs)
{
	/* Update time. */
	self->helpers.time += secs;
}

void
lirnd_render_set_global_shadows (lirndRender* self,
                                 int          value)
{
	self->config.global_shadows = value;
}

int
lirnd_render_get_light_count (const lirndRender* self)
{
	return self->config.light_count;
}

void
lirnd_render_set_light_count (lirndRender* self,
                              int          count)
{
	self->config.light_count = LI_MIN (8, count);
}

void
lirnd_render_set_local_shadows (lirndRender* self,
                                int          value)
{
	self->config.local_shadows = value;
}

int
lirnd_render_get_shaders_enabled (const lirndRender* self)
{
	return self->shader.enabled;
}

void
lirnd_render_set_shaders_enabled (lirndRender* self,
                                  int          value)
{
	if (livid_features.shader_model >= 3)
		self->shader.enabled = value;
	else
		self->shader.enabled = 0;
}

/*****************************************************************************/

static int
private_init_resources (lirndRender* self,
                        const char*  dir)
{
	int x;
	int y;
	unsigned char value;
	unsigned char* pixel;
	unsigned char* pixels;
	const float depth_texture[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const int perlin_grad[16][3] =
	{
		{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},
		{1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},
		{1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0},
		{1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}
	};
	static const int perlin_perm[256] =
	{
		151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
		142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,
		219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,
		68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,
		133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161,1,216,
		80,73,209,76,132,187,208, 89,18,169,200,196,135,130,116,188,159,86,164,
		100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,
		255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,
		119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,22,39,253,
		19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,
		193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,
		214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,
		236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	/* Initialize resource list. */
	self->resources = lirnd_resources_new (self);
	if (self->resources == NULL)
		return 0;

	/* Initialize default depth texture. */
	if (livid_features.shader_model >= 3)
	{
		glGenTextures (1, &self->helpers.depth_texture_max);
		glBindTexture (GL_TEXTURE_2D, self->helpers.depth_texture_max);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2, 2,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, depth_texture);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	/* Initialize noise texture. */
	pixels = lisys_malloc (4 * 256 * 256);
	pixel = pixels;
	if (pixels == NULL)
		return 0;
	for (y = 0 ; y < 256 ; y++)
	{
		for (x = 0 ; x < 256 ; x++)
		{
			value = perlin_perm[(x + perlin_perm[y]) & 0xFF];
			*(pixel++) = perlin_grad[value & 0x0F][0] * 64 + 64;
			*(pixel++) = perlin_grad[value & 0x0F][1] * 64 + 64;
			*(pixel++) = perlin_grad[value & 0x0F][2] * 64 + 64;
			*(pixel++) = value;
		}
	}
	glGenTextures (1, &self->helpers.noise);
	glBindTexture (GL_TEXTURE_2D, self->helpers.noise);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	lisys_free (pixels);

	return 1;
}

static int
private_init_shaders (lirndRender* self)
{
	char* path;
	liarcReader* reader;

	self->shader.fixed = lirnd_shader_new (self);
	if (self->shader.fixed == NULL)
		return 0;
	if (livid_features.shader_model >= 3)
	{
		path = lisys_path_concat (self->config.dir, "shaders", "shadowmap", NULL);
		if (path == NULL)
			return 0;
		reader = liarc_reader_new_from_file (path);
		lisys_free (path);
		if (reader == NULL)
			return 0;
		self->shader.shadowmap = lirnd_shader_new_from_data (self, reader);
		liarc_reader_free (reader);
		if (self->shader.shadowmap == NULL)
			return 0;
	}

	return 1;
}

static liimgTexture*
private_load_image (lirndRender* self,
                    lirndImage*  image)
{
	char* path;
	liimgTexture* result;

	path = lisys_path_format (self->config.dir,
		LISYS_PATH_SEPARATOR, "graphics",
		LISYS_PATH_SEPARATOR, image->name, ".dds", NULL);
	if (path == NULL)
		return NULL;
	result = liimg_texture_new_from_file (path);
	lisys_free (path);

	return result;
}

/** @} */
/** @} */

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
 * \addtogroup LIRenRender Render
 * @{
 */

#include <lipsofsuna/system.h>
#include "render.h"
#include "render-draw.h"
#include "render-group.h"

static int
private_init_resources (LIRenRender* self,
                        const char*  dir);

static int
private_init_shaders (LIRenRender* self);

/*****************************************************************************/

LIRenRender*
liren_render_new (const char* dir)
{
	LIRenRender* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenRender));
	if (self == NULL)
		return NULL;
	self->shader.enabled = (livid_features.shader_model >= 3);

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
	liren_render_free (self);
	return NULL;
}

void
liren_render_free (LIRenRender* self)
{
	LIAlgPtrdicIter iter0;
	LIAlgStrdicIter iter1;
	LIRenImage* image;
	LIRenModel* model;
	LIRenShader* shader;

	/* Free helper resources. */
	glDeleteTextures (1, &self->helpers.noise);
	glDeleteTextures (1, &self->helpers.depth_texture_max);
	if (self->helpers.empty_image != NULL)
		liren_image_free (self->helpers.empty_image);

	/* Free shaders. */
	if (self->shaders != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->shaders)
		{
			shader = iter1.value;
			liren_shader_free (shader);
		}
		lialg_strdic_free (self->shaders);
	}

	/* Free internal shaders. */
	if (self->shader.fixed != NULL)
		liren_shader_free (self->shader.fixed);
	if (self->shader.shadowmap != NULL)
		liren_shader_free (self->shader.shadowmap);

	/* Free models. */
	if (self->models_inst != NULL)
	{
		LIALG_PTRDIC_FOREACH (iter0, self->models_inst)
		{
			model = iter0.value;
			liren_model_free (model);
		}
		lialg_ptrdic_free (self->models_inst);
	}
	if (self->models != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->models)
		{
			model = iter1.value;
			liren_model_free (model);
		}
		lialg_strdic_free (self->models);
	}

	/* Free images. */
	if (self->images != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->images)
		{
			image = iter1.value;
			liren_image_free (image);
		}
		lialg_strdic_free (self->images);
	}

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
LIRenShader*
liren_render_find_shader (LIRenRender* self,
                          const char*  name)
{
	char* path;
	LIRenShader* shader;

	/* Try existing. */
	shader = lialg_strdic_find (self->shaders, name);
	if (shader != NULL)
		return shader;

	/* Try loading. */
	path = lisys_path_format (self->config.dir,
		LISYS_PATH_SEPARATOR, "shaders",
		LISYS_PATH_SEPARATOR, name, NULL);
	if (path == NULL)
		return self->shader.fixed;
	shader = liren_shader_new_from_file (self, path);
	lisys_free (path);

	/* Try fallback. */
	if (shader == NULL)
		shader = liren_shader_new (self);
	if (shader == NULL)
		return self->shader.fixed;

	/* Insert to dictionary. */
	shader->name = listr_dup (name);
	if (shader->name == NULL)
	{
		liren_shader_free (shader);
		return self->shader.fixed;
	}
	if (!lialg_strdic_insert (self->shaders, name, shader))
	{
		liren_shader_free (shader);
		return self->shader.fixed;
	}

	return shader;
}

/**
 * \brief Finds a texture by name.
 *
 * Searches for a texture from the texture cache and returns the match, if any.
 * If no match is found, NULL is returned.
 *
 * \param self Renderer.
 * \param name Name of the texture.
 * \return Texture or NULL.
 */
LIRenImage*
liren_render_find_image (LIRenRender* self,
                         const char*  name)
{
	return lialg_strdic_find (self->images, name);
}

/**
 * \brief Finds a model by name.
 *
 * \param self Renderer.
 * \param name Model name.
 * \return Model.
 */
LIRenModel*
liren_render_find_model (LIRenRender* self,
                         const char*  name)
{
	return lialg_strdic_find (self->models, name);
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
liren_render_load_image (LIRenRender* self,
                         const char*  name)
{
	LIAlgPtrdicIter iter0;
	LIAlgStrdicIter iter1;
	LIRenImage* image;
	LIRenModel* model;

	/* Just create new image if no old one. */
	image = lialg_strdic_find (self->images, name);
	if (image == NULL)
	{
		image = liren_image_new_from_file (self, name);
		if (image == NULL)
		{
			lisys_error_report ();
			image = liren_image_new (self, name);
			if (image == NULL)
				return 0;
		}
		return 1;
	}

	/* Reload existing image. */
	if (!liren_image_load (image))
		return 0;

	/* Replace in all named models. */
	LIALG_STRDIC_FOREACH (iter1, self->models)
	{
		model = iter1.value;
		liren_model_replace_image (model, image);
	}

	/* Replace in all instance models. */
	LIALG_PTRDIC_FOREACH (iter0, self->models_inst)
	{
		model = iter0.value;
		liren_model_replace_image (model, image);
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
liren_render_load_model (LIRenRender* self,
                         const char*  name,
                         LIMdlModel*  model)
{
	LIRenGroup* group;
	LIRenModel* model0;
	LIRenModel* model1;
	LIRenObject* object;
	LIRenScene* scene;
	LIAlgPtrdicIter iter0;
	LIAlgU32dicIter iter1;
	LIAlgPtrdicIter iter2;

	/* Create new model. */
	model0 = lialg_strdic_find (self->models, name);
	model1 = liren_model_new (self, model, name);
	if (model1 == NULL)
		return 0;

	/* Early exit if not reloading. */
	if (model0 == NULL)
		return 1;

	/* Replace in all instances. */
	LIALG_PTRDIC_FOREACH (iter0, self->scenes)
	{
		scene = iter0.value;

		/* Replace in all objects. */
		LIALG_U32DIC_FOREACH (iter1, scene->objects)
		{
			object = iter1.value;
			if (object->model == model0)
			{
				if (!liren_object_set_model (object, model1))
				{
					liren_object_set_model (object, NULL);
					liren_object_set_pose (object, NULL);
				}
			}
		}

		/* Replace in all groups. */
		LIALG_PTRDIC_FOREACH (iter2, scene->groups)
		{
			group = iter2.value;
			liren_group_reload_model (group, model0, model1);
		}
	}

	/* Free old model. */
	liren_model_free (model0);

	return 1;
}

/**
 * \brief Updates the renderer state.
 *
 * \param self Renderer.
 * \param secs Number of seconds since the last update.
 */
void
liren_render_update (LIRenRender* self,
                     float        secs)
{
	/* Update time. */
	self->helpers.time += secs;
}

void
liren_render_set_global_shadows (LIRenRender* self,
                                 int          value)
{
	self->config.global_shadows = value;
}

int
liren_render_get_light_count (const LIRenRender* self)
{
	return self->config.light_count;
}

void
liren_render_set_light_count (LIRenRender* self,
                              int          count)
{
	self->config.light_count = LIMAT_MIN (8, count);
}

void
liren_render_set_local_shadows (LIRenRender* self,
                                int          value)
{
	self->config.local_shadows = value;
}

int
liren_render_get_shaders_enabled (const LIRenRender* self)
{
	return self->shader.enabled;
}

void
liren_render_set_shaders_enabled (LIRenRender* self,
                                  int          value)
{
	if (livid_features.shader_model >= 3)
		self->shader.enabled = value;
	else
		self->shader.enabled = 0;
}

/*****************************************************************************/

#ifndef NDEBUG
void
liren_check_errors ()
{
	switch (glGetError ())
	{
		case GL_NO_ERROR:
			return;
		case GL_INVALID_ENUM:
			fprintf (stderr, "ERROR: GL_INVALID_ENUM\n");
			break;
		case GL_INVALID_VALUE:
			fprintf (stderr, "ERROR: GL_INVALID_VALUE\n");
			break;
		case GL_INVALID_OPERATION:
			fprintf (stderr, "ERROR: GL_INVALID_OPERATION\n");
			break;
		case GL_STACK_OVERFLOW:
			fprintf (stderr, "ERROR: GL_STACK_OVERFLOW\n");
			break;
		case GL_STACK_UNDERFLOW:
			fprintf (stderr, "ERROR: GL_STACK_UNDERFLOW\n");
			break;
		case GL_OUT_OF_MEMORY:
			fprintf (stderr, "ERROR: GL_OUT_OF_MEMORY\n");
			break;
		default:
			fprintf (stderr, "ERROR: Unknown GL error\n");
			break;
	}
	/* assert (0); */
}
#endif

/*****************************************************************************/

static int
private_init_resources (LIRenRender* self,
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

	/* Initialize image dictionary. */
	self->images = lialg_strdic_new ();
	if (self->images == NULL)
		return 0;

	/* Initialize empty image. */
	self->helpers.empty_image = liren_image_new (self, "empty");
	if (self->helpers.empty_image == NULL)
		return 0;

	/* Initialize model dicrionaries. */
	self->models = lialg_strdic_new ();
	if (self->models == NULL)
		return 0;
	self->models_inst = lialg_ptrdic_new ();
	if (self->models_inst == NULL)
		return 0;

	/* Initialize shader dictionary. */
	self->shaders = lialg_strdic_new ();
	if (self->shaders == NULL)
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
private_init_shaders (LIRenRender* self)
{
	char* path;
	LIArcReader* reader;

	self->shader.fixed = liren_shader_new (self);
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
		self->shader.shadowmap = liren_shader_new_from_data (self, reader);
		liarc_reader_free (reader);
		if (self->shader.shadowmap == NULL)
			return 0;
	}

	return 1;
}

/** @} */
/** @} */

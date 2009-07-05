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
 * \addtogroup livie Viewer
 * @{
 * \addtogroup livieViewer Viewer
 * @{
 */

#include <sys/time.h>
#include "viewer.h"

#define ZOOM_SPEED 0.5f
#define ROTATION_SPEED 0.01f

static int
private_init_camera (livieViewer* self);

static int
private_init_engine (livieViewer* self,
                     const char*  name);

static int
private_init_model (livieViewer* self,
                    const char*  model);

static int
private_init_paths (livieViewer* self,
                    const char*  name);

static int
private_init_reload (livieViewer* self);

static int
private_init_video (livieViewer* self);

static int
private_resize (livieViewer* self,
                int          width,
                int          height,
                int          fsaa);

/*****************************************************************************/

livieViewer*
livie_viewer_new (const char* name,
                  const char* model)
{
	char buf[256];
	livieViewer* self;

	/* Initialize SDL. */
	if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1)
	{
		lisys_error_set (ENOTSUP, "initializing SDL failed");
		lisys_error_report ();
		return NULL;
	}

	/* Allocate self. */
	self = calloc (1, sizeof (livieViewer));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		lisys_error_report ();
		return NULL;
	}

	/* Initialize subsystems. */
	if (!private_init_video (self) ||
	    !private_init_paths (self, name) ||
	    !private_init_engine (self, name) ||
		!private_init_reload (self) ||
	    !private_init_camera (self) ||
	    !private_init_model (self, model))
	{
		livie_viewer_free (self);
		lisys_error_report ();
		return NULL;
	}
	snprintf (buf, 256, "%s - Lips of Suna Model Viewer", model);
	SDL_WM_SetCaption (buf, buf);

	return self;
}

void
livie_viewer_free (livieViewer* self)
{
	if (self->lights.key != NULL)
	{
		lirnd_lighting_remove_light (self->engine->scene->lighting, self->lights.key);
		lirnd_light_free (self->lights.key);
	}
	if (self->lights.fill != NULL)
	{
		lirnd_lighting_remove_light (self->engine->scene->lighting, self->lights.fill);
		lirnd_light_free (self->lights.fill);
	}
	if (self->reload != NULL)
		lirel_reload_free (self->reload);
	if (self->camera != NULL)
		lieng_camera_free (self->camera);
	if (self->engine != NULL)
		lieng_engine_free (self->engine);
	if (self->screen != NULL)
		SDL_FreeSurface (self->screen);
	SDL_Quit ();
	free (self->path);
	free (self->datadir);
	free (self);
}

int
livie_viewer_main (livieViewer* self)
{
	float secs;
	struct timeval curr_tick;
	struct timeval prev_tick;
	SDL_Event event;
	limatAabb aabb;
	limatFrustum frustum;
	limatMatrix modelview;
	limatMatrix projection;
	limatTransform transform;

	/* Main loop. */
	gettimeofday (&prev_tick, NULL);
	while (1)
	{
		gettimeofday (&curr_tick, NULL);
		secs = curr_tick.tv_sec - prev_tick.tv_sec +
			  (curr_tick.tv_usec - prev_tick.tv_usec) * 0.000001;
		prev_tick = curr_tick;

		/* Handle events. */
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					return 1;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_s)
					{
						lirnd_render_set_shaders_enabled (self->engine->render,
							!lirnd_render_get_shaders_enabled (self->engine->render));
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 4)
						lieng_camera_zoom (self->camera, -ZOOM_SPEED);
					else if (event.button.button == 5)
						lieng_camera_zoom (self->camera, ZOOM_SPEED);
					break;
				case SDL_MOUSEMOTION:
					if (SDL_GetMouseState (NULL, NULL))
					{
						lieng_camera_turn (self->camera, ROTATION_SPEED * -event.motion.xrel);
						lieng_camera_tilt (self->camera, ROTATION_SPEED * -event.motion.yrel);
					}
					break;
				case SDL_VIDEORESIZE:
					private_resize (self, event.resize.w, event.resize.h, self->mode.fsaa);
					lieng_camera_set_viewport (self->camera, 0, 0, event.resize.w, event.resize.h);
					glViewport (0, 0, event.resize.w, event.resize.h);
					break;
			}
		}

		/* Update resources. */
		lirel_reload_update (self->reload);

		/* Update camera. */
		lieng_object_get_bounds (self->object, &aabb);
		lieng_object_get_transform (self->object, &transform);
		transform.position = limat_vector_add (transform.position,
			limat_vector_multiply (limat_vector_add (aabb.min, aabb.max), 0.5f));
		lieng_camera_set_center (self->camera, &transform);
		lieng_camera_update (self->camera, secs);
		lieng_camera_warp (self->camera);

		/* Update lights. */
		lieng_camera_get_transform (self->camera, &transform);
		transform.position = limat_transform_transform (transform, limat_vector_init (9, 6, -1));
		lirnd_light_set_transform (self->lights.key, &transform);
		lieng_camera_get_transform (self->camera, &transform);
		transform.position = limat_transform_transform (transform, limat_vector_init (-4, 2, 0));
		lirnd_light_set_transform (self->lights.fill, &transform);

		/* Render scene. */
		glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lieng_camera_get_frustum (self->camera, &frustum);
		lieng_camera_get_modelview (self->camera, &modelview);
		lieng_camera_get_projection (self->camera, &projection);
		lirnd_scene_render (self->engine->scene, &modelview, &projection, &frustum);
		SDL_GL_SwapBuffers ();
		SDL_Delay (100);
	}

	return 1;
}

/*****************************************************************************/

static int
private_init_camera (livieViewer* self)
{
	GLint viewport[4];

	self->camera = lieng_camera_new (self->engine);
	if (self->camera == NULL)
		return 0;
	glGetIntegerv (GL_VIEWPORT, viewport);
	lieng_camera_set_driver (self->camera, LIENG_CAMERA_DRIVER_THIRDPERSON);
	lieng_camera_set_viewport (self->camera, viewport[0], viewport[1], viewport[2], viewport[3]);
	lieng_camera_set_clip (self->camera, 0);
	lieng_object_set_realized (self->camera->object, 1);

	return 1;
}

static int
private_init_engine (livieViewer* self,
                     const char*  name)
{
	int flags;
	const float equation[3] = { 1.0f, 0.0f, 0.001f };
	const float diffuse0[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
	const float diffuse1[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	/* Allocate engine. */
	self->engine = lieng_engine_new (self->datadir, self->path, 1);
	if (self->engine == NULL)
		return 0;
	flags = lieng_engine_get_flags (self->engine);
	lieng_engine_set_flags (self->engine, flags | LIENG_FLAG_REMOTE_SECTORS);
	lieng_engine_set_userdata (self->engine, LIENG_DATA_CLIENT, self);
	if (!lieng_engine_load_resources (self->engine, NULL))
		return 0;

	/* Allocate lights. */
	self->lights.key = lirnd_light_new (self->engine->scene, diffuse0, equation, M_PI, 0.0f, 0);
	if (self->lights.key == NULL)
		return 0;
	lirnd_lighting_insert_light (self->engine->scene->lighting, self->lights.key);
	self->lights.fill = lirnd_light_new (self->engine->scene, diffuse1, equation, M_PI, 0.0f, 0);
	if (self->lights.fill == NULL)
		return 0;
	lirnd_lighting_insert_light (self->engine->scene->lighting, self->lights.fill);

	return 1;
}

static int
private_init_model (livieViewer* self,
                    const char*  model)
{
	liengModel* mdl;

	self->object = lieng_object_new (self->engine, NULL,
		LIPHY_SHAPE_MODE_CONVEX, LIPHY_CONTROL_MODE_STATIC, 0, NULL);
	if (self->object == NULL)
		return 0;
	mdl = lieng_engine_find_model_by_name (self->engine, model);
	if (mdl == NULL)
	{
		lisys_error_set (EINVAL, "Cannot find model `%s'", model);
		lisys_error_report ();
		return 1;
	}
	lieng_object_set_model (self->object, mdl);
	lieng_object_set_realized (self->object, 1);

	return 1;
}

static int
private_init_paths (livieViewer* self,
                    const char*  name)
{
	/* Get data directory. */
#ifdef LI_RELATIVE_PATHS
	self->datadir = lisys_relative_exedir (NULL);
#else
	self->datadir = strdup (LIDATADIR);
#endif
	if (self->datadir == NULL)
		return 0;

	/* Get module directory. */
	if (!strcmp (name, "data"))
		self->path = lisys_path_concat (self->datadir, name, NULL);
	else
		self->path = lisys_path_concat (self->datadir, "mods", name, NULL);
	if (self->path == NULL)
		return 0;

	return 1;
}

static int
private_init_reload (livieViewer* self)
{
	self->reload = lirel_reload_new (self->engine);
	if (self->reload == NULL)
		return 0;
	lirel_reload_set_enabled (self->reload, 1);
	lirel_reload_run (self->reload);

	return 1;
}

static int
private_init_video (livieViewer* self)
{
	/* Create the window. */
	if (private_resize (self, 1024, 768, livid_features_get_max_samples ()) +
	    private_resize (self, 1024, 768, 0) == 0)
		return 0;
	livid_features_init ();

	return 1;
}

static int
private_resize (livieViewer* self,
                int          width,
                int          height,
                int          fsaa)
{
	int depth;
	GLenum error;

	/* Recreate surface. */
	for ( ; fsaa >= 0 ; fsaa--)
	{
		for (depth = 32 ; depth ; depth -= 8)
		{
			SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, depth);
			SDL_GL_SetAttribute (SDL_GL_SWAP_CONTROL, 1);
			SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, fsaa? 1 : 0);
			SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, fsaa);
			self->screen = SDL_SetVideoMode (width, height, 0, SDL_OPENGL | SDL_RESIZABLE);
			if (self->screen != NULL)
				break;
		}
		if (self->screen != NULL)
			break;
	}
	if (self->screen == NULL)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "cannot set video mode");
		return 0;
	}

	/* Initialize GLEW. */
	error = glewInit ();
	if (error != GLEW_OK)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "%s", glewGetErrorString (error));
		return 0;
	}

	/* Store mode. */
	self->mode.width = width;
	self->mode.height = height;
	self->mode.fsaa = fsaa;
	if (fsaa)
		glEnable (GL_MULTISAMPLE_ARB);
	else
		glDisable (GL_MULTISAMPLE_ARB);

	return 1;
}

/** @} */
/** @} */

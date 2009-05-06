/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup livid Video
 * @{
 * \addtogroup lividFeatures Features
 * @{
 */

#include <string.h>
#include "video-features.h"
#include "video-opengl.h"
#define __gl_h_
#include <GL/glx.h>
#include <GL/glxext.h>

/**
 * \brief Global video card feature cache.
 */
lividFeatures livid_features;

/**
 * \brief Initializes the global video card feature cache.
 */
void
livid_features_init ()
{
	GLint tmp;

	/* Clear capabilities. */
	memset (&livid_features, 0, sizeof (lividFeatures));

	/* Get capabilities. */
	if (livid_video_check_support ("GL_ARB_depth_buffer_float"))
		livid_features.ARB_depth_buffer_float = 1;
	if (livid_video_check_support ("GL_ARB_depth_texture"))
		livid_features.ARB_depth_texture = 1;
	if (livid_video_check_support ("GL_ARB_point_sprite"))
		livid_features.ARB_point_sprite = 1;
	if (livid_video_check_support ("GL_ARB_pixel_buffer_object"))
		livid_features.ARB_pixel_buffer_object = 1;
	if (livid_video_check_support ("GL_ARB_shadow"))
		livid_features.ARB_shadow = 1;
	if (livid_video_check_support ("GL_ARB_texture_cube_map"))
		livid_features.ARB_texture_cube_map = 1;
	if (livid_video_check_support ("GL_ARB_vertex_buffer_object"))
		livid_features.ARB_vertex_buffer_object = 1;
	if (livid_video_check_support ("GL_EXT_framebuffer_object"))
		livid_features.EXT_framebuffer_object = 1;
	if (livid_video_check_support ("GL_EXT_texture_filter_anisotropic"))
	{
		livid_features.EXT_texture_filter_anisotropic = 1;
		glGetIntegerv (GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &tmp);
		livid_features.anisotropic_level = tmp;
	}
	if (livid_video_check_support ("GL_ARB_shading_language_100"))
	{
		if (livid_video_check_support ("GL_NV_vertex_program3") ||
		    livid_video_check_support ("GL_ATI_shader_texture_lod"))
			livid_features.shader_model = 3;
		else if (livid_video_check_support ("GL_ARB_fragment_shader"))
			livid_features.shader_model = 2;
		else
			livid_features.shader_model = 1;
	}
	glGetIntegerv (GL_MAX_TEXTURE_UNITS, &tmp);
	livid_features.max_texture_units = tmp;
}

/**
 * \brief Gets the absolute maximum of full screen anti-aliasing samples.
 *
 * In practice, the maximum number of samples may depend on context attributes,
 * so you might want to fall back to lower settings if the value doesn't give
 * a functional context. Nevertheless, this should provide a good starting
 * point.
 *
 * \return Number of samples.
 */
int
livid_features_get_max_samples ()
{
	int i;
	int count;
	int screen;
	int attr_buffers;
	int attr_samples;
	int best_samples = 0;
	Display* display;
	GLXFBConfig* configs;

	display = XOpenDisplay (NULL);
	screen = DefaultScreen (display);

	/* Get the best anti-aliasing mode supported by the system. */
	configs = glXGetFBConfigs (display, screen, &count);
	if (configs != NULL)
	{
		best_samples = 0;
		for (i = 0 ; i < count ; i++)
		{
			attr_buffers = 0;
			attr_samples = 0;
			glXGetFBConfigAttrib (display, configs[i], GLX_SAMPLE_BUFFERS_ARB, &attr_buffers);
			glXGetFBConfigAttrib (display, configs[i], GLX_SAMPLES_ARB, &attr_samples);
			if (attr_buffers && attr_samples)
			{
				if (best_samples < attr_samples)
					best_samples = attr_samples;
			}
		}
	}

	XCloseDisplay (display);
	return best_samples;
}

/** @} */
/** @} */

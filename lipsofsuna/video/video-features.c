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
 * \addtogroup livid Video
 * @{
 * \addtogroup LIVidFeatures Features
 * @{
 */

#include <string.h>
#include "video-features.h"
#include "video-opengl.h"
#ifdef HAVE_GL_GLX_H
#include <GL/glx.h>
#include <GL/glxext.h>
#endif

/**
 * \brief Global video card feature cache.
 */
LIVidFeatures livid_features;

/**
 * \brief Initializes the global video card feature cache.
 */
void
livid_features_init ()
{
	GLint tmp;

	/* Clear capabilities. */
	memset (&livid_features, 0, sizeof (LIVidFeatures));

	/* Get capabilities. */
	if (GLEW_EXT_texture_filter_anisotropic)
	{
		glGetIntegerv (GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &tmp);
		livid_features.anisotropic_level = tmp;
	}
	if (GLEW_ARB_shading_language_100)
	{
		if (glewIsSupported ("GL_AMD_vertex_shader_tessellator"))
			livid_features.shader_model = 5;
		else if (glewIsSupported ("GL_ARB_geometry_shader4"))
			livid_features.shader_model = 4;
		else if (GLEW_NV_vertex_program3 || GLEW_ATI_shader_texture_lod ||
		         glewIsSupported ("GL_ARB_shader_texture_lod"))
			livid_features.shader_model = 3;
		else if (GLEW_ARB_fragment_shader)
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
#ifdef HAVE_GL_GLX_H

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
#else

	/* FIXME */
	return 8;

#endif
}

/** @} */
/** @} */

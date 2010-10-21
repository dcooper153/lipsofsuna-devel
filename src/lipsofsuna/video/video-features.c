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
 * \return Nonzero on success.
 */
int livid_features_init ()
{
	GLint tmp;

	/* Clear capabilities. */
	memset (&livid_features, 0, sizeof (LIVidFeatures));

	/* Get capabilities. */
	if (!GLEW_VERSION_3_3)
	{
		lisys_error_set (EINVAL, "OpenGL 3.3 isn't supported by your graphics card");
		return 0;
	}
	if (GLEW_EXT_texture_filter_anisotropic)
	{
		glGetIntegerv (GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &tmp);
		livid_features.anisotropic_level = tmp;
	}

	return 1;
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
int livid_features_get_max_samples ()
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

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

#ifdef DEBUG_VIDEO
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <SDL/SDL.h>
#include "video-opengl.h"

enum
{
	/**
	 * \brief Enables debugging performance of texture fetches.
	 *
	 * All textures are replaced with very small dummy textures. Doing this
	 * eliminates most of the delays caysed by texture lookups.
	 */
	LIVID_DEBUG_TEXTURE = 0x01,

	/**
	 * \brief Enables debugging performance of primitive uploads.
	 *
	 * Causes no primitive data to be uploaded at all. Naturally, doing this
	 * eliminates all delays caused by uploading geometry, furthermore clearly
	 * demonstrating how much time was doing just that.
	 */
	LIVID_DEBUG_PRIMITIVE = 0x02,

	/**
	 * \brief Enables debugging performance of raster operations.
	 *
	 * Forces the viewport size to exactly one pixel, thus eliminating nearly
	 * all raster operations and delays caused by them.
	 */
	LIVID_DEBUG_RASTER = 0x04,

	/**
	 * \brief Enables debugging performance of shaders.
	 *
	 * Forces fixed function pipeline.
	 */
	LIVID_DEBUG_SHADER = 0x08,

	/**
	 * \brief Enables debugging performance of fragment shaders.
	 *
	 * Forces an extremely simple fragment shader that eliminates most of the
	 * delays caused by the fragment shading stage.
	 *
	 * \todo Not supported yet.
	 */
	LIVID_DEBUG_FRAGMENT = 0x10,
};

static int livid_debug_initialized;
static int livid_debug_flags;
static int livid_debug_socket;
static int livid_debug_socket_remote;
static GLuint livid_debug_texture;
static GLubyte livid_debug_texture_pixels[12] =
{
	255, 255, 255,
	128, 128, 128,
	128, 128, 128,
	255, 255, 255,
};

static void self_command (int flags);
static void self_control ();

/****************************************************************************/

#undef livid_video_init
#undef livid_video_free
#undef SDL_GL_SwapBuffers

int livid_debug_init (const char* path)
{
	int flags;
	socklen_t length;
	struct sockaddr_un addr;

	/* Initialize video. */
	if (livid_debug_initialized++)
		return 1;
	if (!livid_video_init (path))
	{
		livid_debug_initialized--;
		return 0;
	}

	/* Create a control socket. */
	livid_debug_socket = socket (AF_UNIX, SOCK_STREAM, 0);
	addr.sun_family = AF_UNIX;
	sprintf (addr.sun_path, "/tmp/livid-debug-%d", getpid ());
	unlink (addr.sun_path);
	length = strlen (addr.sun_path) + sizeof (addr.sun_family);
	bind (livid_debug_socket, (struct sockaddr*) &addr, length);
	listen (livid_debug_socket, 1);
	flags = fcntl (livid_debug_socket, F_GETFL, 0);
	fcntl (livid_debug_socket, F_SETFL, flags | O_NONBLOCK);
	printf ("VIDEO DEBUG: Process number %d.\n", getpid ());

	/* Clear the rest. */
	livid_debug_socket_remote = -1;
	livid_debug_texture = 0;
	return 1;
}

void livid_debug_free ()
{
	if (--livid_debug_initialized)
		return;

	/* Close the control socket. */
	if (livid_debug_socket_remote != -1)
	{
		close (livid_debug_socket_remote);
		livid_debug_socket_remote = -1;
	}
	close (livid_debug_socket);
	livid_debug_socket = -1;

	/* Close video. */
	livid_video_free ();
}

/****************************************************************************/

void livid_debug_glBindTexture (GLenum target, GLuint texture)
{
	if (livid_debug_flags & LIVID_DEBUG_TEXTURE)
	{
		if (texture)
		{
			if (!livid_debug_texture)
			{
				glGenTextures (1, &livid_debug_texture);
				glBindTexture (GL_TEXTURE_2D, livid_debug_texture);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexImage2D (GL_TEXTURE_2D, 0, 3, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, livid_debug_texture_pixels);
			}
			else
				glBindTexture (GL_TEXTURE_2D, livid_debug_texture);
		}
		else
			glBindTexture (GL_TEXTURE_2D, 0);
	}
	else
		glBindTexture (target, texture);
}

void livid_debug_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glDrawArrays (mode, first, count);
}

void livid_debug_glNormal3b(GLbyte nx, GLbyte ny, GLbyte nz)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glNormal3b (nx, ny, nz);
}

void livid_debug_glNormal3bv(const GLbyte *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glNormal3bv (v);
}

void livid_debug_glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glNormal3d (nx, ny, nz);
}

void livid_debug_glNormal3dv(const GLdouble *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glNormal3dv (v);
}

void livid_debug_glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glNormal3f (nx, ny, nz);
}

void livid_debug_glNormal3fv(const GLfloat *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glNormal3fv (v);
}

void livid_debug_glNormal3i(GLint nx, GLint ny, GLint nz)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glNormal3i (nx, ny, nz);
}

void livid_debug_glNormal3iv(const GLint *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glNormal3iv (v);
}

void livid_debug_glNormal3s(GLshort nx, GLshort ny, GLshort nz)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glNormal3s (nx, ny, nz);
}

void livid_debug_glNormal3sv(const GLshort *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glNormal3sv (v);
}

void livid_debug_glUseProgram(GLuint program)
{
	if (!(livid_debug_flags & LIVID_DEBUG_SHADER))
		glUseProgram (program);
	else
		glUseProgram (0);
}

void livid_debug_glVertex2d(GLdouble x, GLdouble y)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex2d (x, y);
}

void livid_debug_glVertex2dv(const GLdouble *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex2dv (v);
}

void livid_debug_glVertex2f(GLfloat x, GLfloat y)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex2f (x, y);
}

void livid_debug_glVertex2fv(const GLfloat *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex2fv (v);
}

void livid_debug_glVertex2i(GLint x, GLint y)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex2i (x, y);
}

void livid_debug_glVertex2iv(const GLint *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex2iv (v);
}

void livid_debug_glVertex2s(GLshort x, GLshort y)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex2s (x, y);
}

void livid_debug_glVertex2sv(const GLshort *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex2sv (v);
}

void livid_debug_glVertex3d(GLdouble x, GLdouble y, GLdouble z)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex3d (x, y, z);
}

void livid_debug_glVertex3dv(const GLdouble *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex3dv (v);
}

void livid_debug_glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex3f (x, y, z);
}

void livid_debug_glVertex3fv(const GLfloat *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex3fv (v);
}

void livid_debug_glVertex3i(GLint x, GLint y, GLint z)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex3i (x, y, z);
}

void livid_debug_glVertex3iv(const GLint *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex3iv (v);
}

void livid_debug_glVertex3s(GLshort x, GLshort y, GLshort z)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex3s (x, y, z);
}

void livid_debug_glVertex3sv(const GLshort *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex3sv (v);
}

void livid_debug_glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex4d (x, y, z, w);
}

void livid_debug_glVertex4dv(const GLdouble *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex4dv (v);
}

void livid_debug_glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex4f (x, y, z, w);
}

void livid_debug_glVertex4fv(const GLfloat *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex4fv (v);
}

void livid_debug_glVertex4i(GLint x, GLint y, GLint z, GLint w)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex4i (x, y, z, w);
}

void livid_debug_glVertex4iv(const GLint *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex4iv (v);
}

void livid_debug_glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex4s (x, y, z, w);
}

void livid_debug_glVertex4sv(const GLshort *v)
{
	if (!(livid_debug_flags & LIVID_DEBUG_PRIMITIVE))
		glVertex4sv (v);
}

void livid_debug_glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	if (!(livid_debug_flags & LIVID_DEBUG_RASTER))
		glViewport (x, y, width, height);
}

void livid_debug_SDL_GL_SwapBuffers()
{
	self_control ();
	SDL_GL_SwapBuffers ();
}

/****************************************************************************/

static void self_command (int flags)
{
	livid_debug_flags = flags;

	if (flags & LIVID_DEBUG_TEXTURE)
	{
		livid_debug_glBindTexture (GL_TEXTURE_2D, 1);
	}
	if (flags & LIVID_DEBUG_PRIMITIVE)
	{
	}
	if (flags & LIVID_DEBUG_RASTER)
	{
		glViewport (0, 0, 1, 1);
	}
	if (flags & LIVID_DEBUG_FRAGMENT)
	{
		printf ("VIDEO DEBUG: DEBUG_FRAGMENT not implemented yet.\n");
	}
}

static void self_control ()
{
	char cmd;
	int ret;
	socklen_t length;
	struct sockaddr_un addr;

	/* Accept connections. */
	if (livid_debug_socket_remote == -1)
	{
		length = sizeof(struct sockaddr_un);
		livid_debug_socket_remote = accept (livid_debug_socket, &addr, &length);
		if (livid_debug_socket_remote != -1)
			printf ("VIDEO DEBUG: Remote connected.\n");
	}

	/* Read commands. */
	if (livid_debug_socket_remote != -1)
	{
		ret = recv (livid_debug_socket_remote, &cmd, 1, MSG_DONTWAIT);
		switch (ret)
		{
			case -1:
				if (errno == EAGAIN)
					break;
			case 0:
				close (livid_debug_socket_remote);
				livid_debug_socket_remote = -1;
				printf ("VIDEO DEBUG: Remote disconnected.\n");
				return;
			default:
				livid_debug_flags = cmd;
				printf ("VIDEO DEBUG: Remote command %d.\n", cmd);
				self_command (cmd);
				break;
		}
	}

	/* Send framerate ticks. */
	if (livid_debug_socket_remote != -1)
	{
		cmd = 1;
		ret = send (livid_debug_socket_remote, &cmd, 1, MSG_NOSIGNAL);
		if (ret <= 0)
		{
			close (livid_debug_socket_remote);
			livid_debug_socket_remote = -1;
			printf ("VIDEO DEBUG: Remote disconnected.\n");
			return;
		}
	}
}

#endif


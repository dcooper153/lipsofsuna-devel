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
#ifndef __VIDEO_DEBUG_H__
#define __VIDEO_DEGUG_H__

#ifdef __VIDEO_OPENGL_H__
#error "video-debug.h must be included before video-opengl.h"
#else
#include "video-opengl.h"
#endif

#undef glBindTexture
#undef glDrawArrays
#undef glNormal3b
#undef glNormal3bv
#undef glNormal3d
#undef glNormal3dv
#undef glNormal3f
#undef glNormal3fv
#undef glNormal3i
#undef glNormal3iv
#undef glNormal3s
#undef glNormal3sv
#undef glUseProgram
#undef glVertex2d
#undef glVertex2dv
#undef glVertex2f
#undef glVertex2fv
#undef glVertex2i
#undef glVertex2iv
#undef glVertex2s
#undef glVertex2sv
#undef glVertex3d
#undef glVertex3dv
#undef glVertex3f
#undef glVertex3fv
#undef glVertex3i
#undef glVertex3iv
#undef glVertex3s
#undef glVertex3sv
#undef glVertex4d
#undef glVertex4dv
#undef glVertex4f
#undef glVertex4fv
#undef glVertex4i
#undef glVertex4iv
#undef glVertex4s
#undef glVertex4sv
#undef glViewport

int  livid_debug_init (const char* path);
void livid_debug_free ();
void livid_debug_glBindTexture (GLenum target, GLuint texture);
void livid_debug_glDrawArrays(GLenum mode, GLint first, GLsizei count);
void livid_debug_glNormal3b(GLbyte nx, GLbyte ny, GLbyte nz);
void livid_debug_glNormal3bv(const GLbyte *v);
void livid_debug_glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz);
void livid_debug_glNormal3dv(const GLdouble *v);
void livid_debug_glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
void livid_debug_glNormal3fv(const GLfloat *v);
void livid_debug_glNormal3i(GLint nx, GLint ny, GLint nz);
void livid_debug_glNormal3iv(const GLint *v);
void livid_debug_glNormal3s(GLshort nx, GLshort ny, GLshort nz);
void livid_debug_glNormal3sv(const GLshort *v);
void livid_debug_glUseProgram(GLuint program);
void livid_debug_glVertex2d(GLdouble x, GLdouble y);
void livid_debug_glVertex2dv(const GLdouble *v);
void livid_debug_glVertex2f(GLfloat x, GLfloat y);
void livid_debug_glVertex2fv(const GLfloat *v);
void livid_debug_glVertex2i(GLint x, GLint y);
void livid_debug_glVertex2iv(const GLint *v);
void livid_debug_glVertex2s(GLshort x, GLshort y);
void livid_debug_glVertex2sv(const GLshort *v);
void livid_debug_glVertex3d(GLdouble x, GLdouble y, GLdouble z);
void livid_debug_glVertex3dv(const GLdouble *v);
void livid_debug_glVertex3f(GLfloat x, GLfloat y, GLfloat z);
void livid_debug_glVertex3fv(const GLfloat *v);
void livid_debug_glVertex3i(GLint x, GLint y, GLint z);
void livid_debug_glVertex3iv(const GLint *v);
void livid_debug_glVertex3s(GLshort x, GLshort y, GLshort z);
void livid_debug_glVertex3sv(const GLshort *v);
void livid_debug_glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void livid_debug_glVertex4dv(const GLdouble *v);
void livid_debug_glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void livid_debug_glVertex4fv(const GLfloat *v);
void livid_debug_glVertex4i(GLint x, GLint y, GLint z, GLint w);
void livid_debug_glVertex4iv(const GLint *v);
void livid_debug_glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w);
void livid_debug_glVertex4sv(const GLshort *v);
void livid_debug_glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
void livid_debug_SDL_GL_SwapBuffers();

#define glBindTexture livid_debug_glBindTexture
#define glDrawArrays livid_debug_glDrawArrays
#define glNormal3b livid_debug_glNormal3b
#define glNormal3bv livid_debug_glNormal3bv
#define glNormal3d livid_debug_glNormal3d
#define glNormal3dv livid_debug_glNormal3dv
#define glNormal3f livid_debug_glNormal3f
#define glNormal3fv livid_debug_glNormal3fv
#define glNormal3i livid_debug_glNormal3i
#define glNormal3iv livid_debug_glNormal3iv
#define glNormal3s livid_debug_glNormal3s
#define glNormal3sv livid_debug_glNormal3sv
#define glUseProgram livid_debug_glUseProgram
#define glVertex2d livid_debug_glVertex2d
#define glVertex2dv livid_debug_glVertex2dv
#define glVertex2f livid_debug_glVertex2f
#define glVertex2fv livid_debug_glVertex2fv
#define glVertex2i livid_debug_glVertex2i
#define glVertex2iv livid_debug_glVertex2iv
#define glVertex2s livid_debug_glVertex2s
#define glVertex2sv livid_debug_glVertex2sv
#define glVertex3d livid_debug_glVertex3d
#define glVertex3dv livid_debug_glVertex3dv
#define glVertex3f livid_debug_glVertex3f
#define glVertex3fv livid_debug_glVertex3fv
#define glVertex3i livid_debug_glVertex3i
#define glVertex3iv livid_debug_glVertex3iv
#define glVertex3s livid_debug_glVertex3s
#define glVertex3sv livid_debug_glVertex3sv
#define glVertex4d livid_debug_glVertex4d
#define glVertex4dv livid_debug_glVertex4dv
#define glVertex4f livid_debug_glVertex4f
#define glVertex4fv livid_debug_glVertex4fv
#define glVertex4i livid_debug_glVertex4i
#define glVertex4iv livid_debug_glVertex4iv
#define glVertex4s livid_debug_glVertex4s
#define glVertex4sv livid_debug_glVertex4sv
#define glViewport livid_debug_glViewport
#define SDL_GL_SwapBuffers livid_debug_SDL_GL_SwapBuffers

#define livid_video_init livid_debug_init
#define livid_video_free livid_debug_free

#endif
#endif


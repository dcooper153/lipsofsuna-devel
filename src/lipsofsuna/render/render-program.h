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

#ifndef __RENDER_PROGRAM_H__
#define __RENDER_PROGRAM_H__

#include <lipsofsuna/system.h>
#include "render-attribute.h"
#include "render-types.h"
#include "render-uniform.h"

typedef struct _LIRenProgram LIRenProgram;
struct _LIRenProgram
{
	LIRenRender* render;
	GLuint program;
	GLuint vertex;
	GLuint geometry;
	GLuint fragment;
};

LIAPICALL (int, liren_program_init, (
	LIRenProgram* self,
	LIRenRender*  render));

LIAPICALL (void, liren_program_clear, (
	LIRenProgram* self));

LIAPICALL (int, liren_program_compile, (
	LIRenProgram* self,
	const char*   name,
	const char*   vertex,
	const char*   geometry,
	const char*   fragment,
	int           feedback));

#endif


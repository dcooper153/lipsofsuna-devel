/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#ifndef __EXT_MATH_MODULE_H__
#define __EXT_MATH_MODULE_H__

#include "lipsofsuna/extension.h"

#define LIEXT_SCRIPT_MATH "Math"

typedef struct _LIExtMathModule LIExtMathModule;
struct _LIExtMathModule
{
	LIMaiProgram* program;
};

LIExtMathModule* liext_math_new (
	LIMaiProgram* program);

void liext_math_free (
	LIExtMathModule* self);

/*****************************************************************************/

void liext_script_quaternion (
	LIScrScript* self);

void liext_script_vector (
	LIScrScript* self);

#endif

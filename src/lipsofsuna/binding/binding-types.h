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

#ifndef __BINDING_TYPES_H__
#define __BINDING_TYPES_H__

typedef struct _LIBndBinding LIBndBinding;
typedef enum _LIBndType LIBndType;
enum _LIBndType
{
	LIBND_TYPE_KEYBOARD,
	LIBND_TYPE_MOUSE,
	LIBND_TYPE_MOUSE_AXIS,
	LIBND_TYPE_MOUSE_DELTA,
	LIBND_TYPE_JOYSTICK,
	LIBND_TYPE_JOYSTICK_AXIS,
	LIBND_TYPE_MAX
};

typedef struct _LIBndAction LIBndAction;
typedef int (*libndCallback)(LIBndAction*  action,
                             LIBndBinding* binding,
                             float         value,
                             void*         data);


typedef struct _LIBndManager LIBndManager;

#endif

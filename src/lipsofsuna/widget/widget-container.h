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

#ifndef __WIDGET_CONTAINER_H__
#define __WIDGET_CONTAINER_H__

#include <lipsofsuna/system.h>
#include "widget.h"
#include "widget-types.h"

#define LIWDG_CONTAINER(o) ((LIWdgContainer*)(o))

typedef struct _LIWdgContainer LIWdgContainer;
typedef struct _LIWdgContainerIface LIWdgContainerIface;
typedef LIWdgWidget* (*LIWdgContainerChildAtFunc)(LIWdgContainer*, int, int);
typedef void (*LIWdgContainerChildRequestFunc)(LIWdgContainer*, LIWdgWidget*);
typedef LIWdgWidget* (*LIWdgContainerCycleFocusFunc)(LIWdgContainer*, LIWdgWidget*, int);
typedef void (*LIWdgContainerDetachChildFunc)(LIWdgContainer*, LIWdgWidget*);
typedef void (*LIWdgContainerForeachChildFunc)(LIWdgContainer*, void (*)(), void*);
typedef void (*LIWdgContainerTranslateCoordsFunc)(LIWdgContainer*, int, int, int*, int*);

struct _LIWdgContainer
{
	LIWdgWidget base;
};

struct _LIWdgContainerIface
{
	LIWdgContainerChildAtFunc child_at;
	LIWdgContainerChildRequestFunc child_request;
	LIWdgContainerCycleFocusFunc cycle_focus;
	LIWdgContainerDetachChildFunc detach_child;
	LIWdgContainerForeachChildFunc foreach_child;
	LIWdgContainerTranslateCoordsFunc translate_coords;
};

LIAPICALL (const LIWdgClass*, liwdg_widget_container, ());

LIAPICALL (LIWdgWidget*, liwdg_container_child_at, (
	LIWdgContainer* self,
	int             x,
	int             y));

LIAPICALL (void, liwdg_container_child_request, (
	LIWdgContainer* self,
	LIWdgWidget*    child));

LIAPICALL (LIWdgWidget*, liwdg_container_cycle_focus, (
	LIWdgContainer* self,
	LIWdgWidget*    curr,
	int             dir));

LIAPICALL (void, liwdg_container_detach_child, (
	LIWdgContainer* self,
	LIWdgWidget*    child));

LIAPICALL (void, liwdg_container_foreach_child, (
	LIWdgContainer* self,
	void          (*call)(),
	void*           data));

LIAPICALL (void, liwdg_container_paint_children, (
	LIWdgContainer* self));

LIAPICALL (void, liwdg_container_translate_coords, (
	LIWdgContainer* self,
	int             containerx,
	int             containery,
	int*            childx,
	int*            childy));

#endif

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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup liwdgContainer Container
 * @{
 */

#ifndef __WIDGET_CONTAINER_H__
#define __WIDGET_CONTAINER_H__

#include "widget.h"
#include "widget-types.h"

#define LIWDG_CONTAINER(o) ((liwdgContainer*)(o))

typedef struct _liwdgContainer liwdgContainer;
typedef struct _liwdgContainerIface liwdgContainerIface;
typedef liwdgWidget* (*liwdgContainerChildAtFunc)(liwdgContainer*, int, int);
typedef void (*liwdgContainerChildRequestFunc)(liwdgContainer*, liwdgWidget*);
typedef liwdgWidget* (*liwdgContainerCycleFocusFunc)(liwdgContainer*, liwdgWidget*, int);
typedef void (*liwdgContainerDetachChildFunc)(liwdgContainer*, liwdgWidget*);
typedef void (*liwdgContainerForeachChildFunc)(liwdgContainer*, void (*)(), void*);
typedef void (*liwdgContainerTranslateCoordsFunc)(liwdgContainer*, int, int, int*, int*);

struct _liwdgContainer
{
	liwdgWidget base;
};

struct _liwdgContainerIface
{
	liwdgContainerChildAtFunc child_at;
	liwdgContainerChildRequestFunc child_request;
	liwdgContainerCycleFocusFunc cycle_focus;
	liwdgContainerDetachChildFunc detach_child;
	liwdgContainerForeachChildFunc foreach_child;
	liwdgContainerTranslateCoordsFunc translate_coords;
};

extern const liwdgClass liwdgContainerType;

liwdgWidget*
liwdg_container_child_at (liwdgContainer* self,
                          int             x,
                          int             y);

void
liwdg_container_child_request (liwdgContainer* self,
                               liwdgWidget*    child);

liwdgWidget*
liwdg_container_cycle_focus (liwdgContainer* self,
                             liwdgWidget*    curr,
                             int             dir);

void
liwdg_container_detach_child (liwdgContainer* self,
                              liwdgWidget*    child);

void
liwdg_container_foreach_child (liwdgContainer* self,
                               void          (*call)(),
                               void*           data);

void
liwdg_container_translate_coords (liwdgContainer* self,
                                  int             containerx,
                                  int             containery,
                                  int*            childx,
                                  int*            childy);

#endif

/** @} */
/** @} */

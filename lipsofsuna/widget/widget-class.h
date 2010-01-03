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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup LIWdgClass Class
 * @{
 */

#ifndef __WIDGET_CLASS_H__
#define __WIDGET_CLASS_H__

#include "widget-event.h"
#include "widget-types.h"

#define LIWDG_WIDGET_CLASS(c) ((LIWdgWidgetClass*)(c))

typedef int  (*LIWdgWidgetInitFunc)(LIWdgWidget*, LIWdgManager*);
typedef void (*LIWdgWidgetFreeFunc)(LIWdgWidget*);
typedef int  (*LIWdgWidgetEventFunc)(LIWdgWidget*, liwdgEvent*);
typedef void* (*LIWdgWidgetExtendFunc)(LIWdgWidget*, LIWdgClass* clss);

struct _LIWdgClass
{
	int basetype;
	const void* base;
	const char* type;
	int size;
	LIWdgWidgetInitFunc init;
	LIWdgWidgetFreeFunc free;
	LIWdgWidgetEventFunc event;
	LIWdgWidgetExtendFunc extend;
};

/**
 * \brief Gets the base class of the class.
 *
 * \param self Class.
 * \return Class or NULL.
 */
static inline const LIWdgClass*
liwdg_class_get_base (const LIWdgClass* self)
{
	const void* (*func)();

	if (self->base == NULL)
		return NULL;
	if (self->basetype == LIWDG_BASE_DYNAMIC)
	{
		func = self->base;
		return func ();
	}
	else
	{
		assert (self->basetype == LIWDG_BASE_STATIC);
		return self->base;
	}
}

#endif

/** @} */
/** @} */
/** @} */

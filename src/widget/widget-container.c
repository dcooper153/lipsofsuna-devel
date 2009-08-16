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

#include "widget-container.h"

static int
private_init (liwdgContainer* self,
              liwdgManager*   manager);

static void
private_free (liwdgContainer* self);

static int
private_event (liwdgContainer* self,
               liwdgEvent*     event);

/*****************************************************************************/

const liwdgClass liwdgContainerType =
{
	LIWDG_BASE_STATIC, &liwdgWidgetType, "Container", sizeof (liwdgContainer),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event,
};

/**
 * \brief Gets a child widget under the cursor position.
 *
 * \param self Container.
 * \param pixx Cursor position.
 * \param pixy Cursor position.
 * \return Widget owned by the group or NULL.
 */
liwdgWidget*
liwdg_container_child_at (liwdgContainer* self,
                          int             x,
                          int             y)
{
	liwdgEvent event;
	liwdgContainerIface* iface;

	/* Probe interface. */
	event.type = LIWDG_EVENT_TYPE_PROBE;
	event.probe.clss = &liwdgContainerType;
	event.probe.result = NULL;
	liwdg_widget_event (LIWDG_WIDGET (self), &event);
	iface = event.probe.result;
	assert (iface != NULL);
	if (iface == NULL)
		return NULL;

	/* Call interface. */
	if (iface->child_at == NULL)
		return iface->child_at (self, x, y);

	return NULL;
}

/**
 * \brief Updates the layout after the size of a child has changed.
 *
 * \param self Container.
 * \param child Child widget.
 */
void
liwdg_container_child_request (liwdgContainer* self,
                               liwdgWidget*    child)
{
	liwdgEvent event;
	liwdgContainerIface* iface;

	/* Probe interface. */
	event.type = LIWDG_EVENT_TYPE_PROBE;
	event.probe.clss = &liwdgContainerType;
	event.probe.result = NULL;
	liwdg_widget_event (LIWDG_WIDGET (self), &event);
	iface = event.probe.result;
	assert (iface != NULL);
	if (iface == NULL)
		return;

	/* Call interface. */
	if (iface->child_request != NULL)
		iface->child_request (self, child);
}

liwdgWidget*
liwdg_container_cycle_focus (liwdgContainer* self,
                             liwdgWidget*    curr,
                             int             dir)
{
	liwdgEvent event;
	liwdgContainerIface* iface;

	/* Probe interface. */
	event.type = LIWDG_EVENT_TYPE_PROBE;
	event.probe.clss = &liwdgContainerType;
	event.probe.result = NULL;
	liwdg_widget_event (LIWDG_WIDGET (self), &event);
	iface = event.probe.result;
	assert (iface != NULL);
	if (iface == NULL)
		return NULL;

	/* Call interface. */
	if (iface->cycle_focus != NULL)
		return iface->cycle_focus (self, curr, dir);

	return NULL;
}

/**
 * \brief Finds and unparents a child widget.
 *
 * \param self Container.
 * \param child Child widget.
 */
void
liwdg_container_detach_child (liwdgContainer* self,
                              liwdgWidget*    child)
{
	liwdgEvent event;
	liwdgContainerIface* iface;

	/* Probe interface. */
	event.type = LIWDG_EVENT_TYPE_PROBE;
	event.probe.clss = &liwdgContainerType;
	event.probe.result = NULL;
	liwdg_widget_event (LIWDG_WIDGET (self), &event);
	iface = event.probe.result;
	assert (iface != NULL);
	if (iface == NULL)
		return;

	/* Call interface. */
	if (iface->detach_child != NULL)
		iface->detach_child (self, child);
}

/**
 * \brief Calls the passed function for each child.
 *
 * \param self Container.
 * \param call Function to call.
 * \param data Data to pass to the function.
 */
void
liwdg_container_foreach_child (liwdgContainer* self,
                               void          (*call)(),
                               void*           data)
{
	liwdgEvent event;
	liwdgContainerIface* iface;

	/* Probe interface. */
	event.type = LIWDG_EVENT_TYPE_PROBE;
	event.probe.clss = &liwdgContainerType;
	event.probe.result = NULL;
	liwdg_widget_event (LIWDG_WIDGET (self), &event);
	iface = event.probe.result;
	assert (iface != NULL);
	if (iface == NULL)
		return;

	/* Call interface. */
	if (iface->foreach_child != NULL)
		iface->foreach_child (self, call, data);
}

/*****************************************************************************/

static int
private_init (liwdgContainer* self,
              liwdgManager*   manager)
{
	return 1;
}

static void
private_free (liwdgContainer* self)
{
}

static int
private_event (liwdgContainer* self,
               liwdgEvent*     event)
{
	/* Container interface. */
	if (event->type == LIWDG_EVENT_TYPE_PROBE &&
	    event->probe.clss == &liwdgContainerType)
	{
		static liwdgContainerIface iface =
		{
			(liwdgContainerChildAtFunc) NULL,
			(liwdgContainerChildRequestFunc) NULL,
			(liwdgContainerCycleFocusFunc) NULL,
			(liwdgContainerDetachChildFunc) NULL,
			(liwdgContainerForeachChildFunc) NULL
		};
		event->probe.result = &iface;
		return 0;
	}

	return liwdgWidgetType.event (LIWDG_WIDGET (self), event);
}

/** @} */
/** @} */

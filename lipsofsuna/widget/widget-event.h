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
 * \addtogroup liwdgEvent Event
 * @{
 */

#ifndef __WIDGET_EVENT_H__
#define __WIDGET_EVENT_H__

#include <system/lips-system.h>
#include "widget-types.h"

enum
{
	LIWDG_EVENT_TYPE_ALLOCATION,
	LIWDG_EVENT_TYPE_BUTTON_PRESS,
	LIWDG_EVENT_TYPE_BUTTON_RELEASE,
	LIWDG_EVENT_TYPE_CLOSE,
	LIWDG_EVENT_TYPE_FOCUS_GAIN,
	LIWDG_EVENT_TYPE_FOCUS_LOSE,
	LIWDG_EVENT_TYPE_KEY_PRESS,
	LIWDG_EVENT_TYPE_KEY_RELEASE,
	LIWDG_EVENT_TYPE_MOTION,
	LIWDG_EVENT_TYPE_PROBE,
	LIWDG_EVENT_TYPE_RENDER,
	LIWDG_EVENT_TYPE_UPDATE,
	LIWDG_EVENT_TYPE_MAX,
};

typedef union _liwdgEvent liwdgEvent;
typedef struct _liwdgEventAllocation liwdgEventAllocation;
typedef struct _liwdgEventButton liwdgEventButton;
typedef struct _liwdgEventFocus liwdgEventFocus;
typedef struct _liwdgEventKey liwdgEventKey;
typedef struct _liwdgEventMotion liwdgEventMotion;
typedef struct _liwdgEventProbe liwdgEventProbe;
typedef struct _liwdgEventUpdate liwdgEventUpdate;

struct _liwdgEventAllocation
{
	int type;
};

struct _liwdgEventButton
{
	int type;
	int x;
	int y;
	int button;
};

struct _liwdgEventFocus
{
	int type;
	int mouse;
};

struct _liwdgEventKey
{
	int type;
	int keycode;
	uint32_t unicode;
	uint32_t modifiers;
};

struct _liwdgEventMotion
{
	int type;
	int x;
	int y;
	int dx;
	int dy;
	int buttons;
};

struct _liwdgEventProbe
{
	int type;
	const liwdgClass* clss;
	void* result;
};

struct _liwdgEventUpdate
{
	int type;
	float secs;
};

union _liwdgEvent
{
	int type;
	liwdgEventAllocation allocation;
	liwdgEventButton button;
	liwdgEventFocus focus;
	liwdgEventKey key;
	liwdgEventMotion motion;
	liwdgEventProbe probe;
	liwdgEventUpdate update;
};

#endif

/** @} */
/** @} */


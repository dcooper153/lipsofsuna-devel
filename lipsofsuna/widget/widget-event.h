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
 * \addtogroup liwdgEvent Event
 * @{
 */

#ifndef __WIDGET_EVENT_H__
#define __WIDGET_EVENT_H__

#include <lipsofsuna/system.h>
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
typedef struct _LIWdgEventAllocation LIWdgEventAllocation;
typedef struct _LIWdgEventButton LIWdgEventButton;
typedef struct _LIWdgEventFocus LIWdgEventFocus;
typedef struct _LIWdgEventKey LIWdgEventKey;
typedef struct _LIWdgEventMotion LIWdgEventMotion;
typedef struct _LIWdgEventProbe LIWdgEventProbe;
typedef struct _LIWdgEventUpdate LIWdgEventUpdate;

struct _LIWdgEventAllocation
{
	int type;
};

struct _LIWdgEventButton
{
	int type;
	int x;
	int y;
	int button;
};

struct _LIWdgEventFocus
{
	int type;
	int mouse;
};

struct _LIWdgEventKey
{
	int type;
	int keycode;
	uint32_t unicode;
	uint32_t modifiers;
};

struct _LIWdgEventMotion
{
	int type;
	int x;
	int y;
	int dx;
	int dy;
	int buttons;
};

struct _LIWdgEventProbe
{
	int type;
	const LIWdgClass* clss;
	void* result;
};

struct _LIWdgEventUpdate
{
	int type;
	float secs;
};

union _liwdgEvent
{
	int type;
	LIWdgEventAllocation allocation;
	LIWdgEventButton button;
	LIWdgEventFocus focus;
	LIWdgEventKey key;
	LIWdgEventMotion motion;
	LIWdgEventProbe probe;
	LIWdgEventUpdate update;
};

#endif

/** @} */
/** @} */


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
typedef struct _liwdgEventUpdate liwdgEventUpdate;

struct _liwdgEventAllocation
{
	uint32_t type;
};

struct _liwdgEventButton
{
	uint32_t type;
	uint32_t x;
	uint32_t y;
	uint8_t  button;
};

struct _liwdgEventFocus
{
	uint32_t type;
	uint8_t mouse;
};

struct _liwdgEventKey
{
	uint32_t type;
	uint32_t keycode;
	uint32_t unicode;
	uint32_t modifiers;
};

struct _liwdgEventMotion
{
	uint32_t type;
	uint32_t x;
	uint32_t y;
};

struct _liwdgEventUpdate
{
	uint32_t type;
	float secs;
};

union _liwdgEvent
{
	uint32_t type;
	liwdgEventAllocation allocation;
	liwdgEventButton button;
	liwdgEventFocus focus;
	liwdgEventKey key;
	liwdgEventMotion motion;
	liwdgEventUpdate update;
};

#endif

/** @} */
/** @} */


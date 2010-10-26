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

#ifndef __WIDGET_EVENT_H__
#define __WIDGET_EVENT_H__

#include <lipsofsuna/system.h>
#include "widget-types.h"

enum
{
	LIWDG_EVENT_TYPE_BUTTON_PRESS,
	LIWDG_EVENT_TYPE_BUTTON_RELEASE,
	LIWDG_EVENT_TYPE_KEY_PRESS,
	LIWDG_EVENT_TYPE_KEY_RELEASE,
	LIWDG_EVENT_TYPE_MOTION,
	LIWDG_EVENT_TYPE_MAX,
};

typedef union _LIWdgEvent LIWdgEvent;
typedef struct _LIWdgEventButton LIWdgEventButton;
typedef struct _LIWdgEventKey LIWdgEventKey;
typedef struct _LIWdgEventMotion LIWdgEventMotion;

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

union _LIWdgEvent
{
	int type;
	LIWdgEventButton button;
	LIWdgEventKey key;
	LIWdgEventMotion motion;
};

#endif

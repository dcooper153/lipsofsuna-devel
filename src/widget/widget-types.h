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

#ifndef __WIDGET_TYPES_H__
#define __WIDGET_TYPES_H__

#include <image/lips-image.h>

#define LIWDG_HANDLER(c) ((liwdgHandler)(c))

enum
{
	LIWDG_CALLBACK_ACTIVATED,
	LIWDG_CALLBACK_PRESSED,
	LIWDG_CALLBACK_LAST
};

enum
{
	LIWDG_WIDGET_STATE_DETACHED,
	LIWDG_WIDGET_STATE_POPUP,
	LIWDG_WIDGET_STATE_ROOT,
	LIWDG_WIDGET_STATE_WINDOW
};

typedef int (*liwdgHandler)();
typedef int liwdgWidgetState;
typedef struct _liwdgManager liwdgManager;
typedef struct _liwdgWidget liwdgWidget;
typedef struct _liwdgWidgetClass liwdgWidgetClass;
typedef struct _liwdgRect liwdgRect;
typedef struct _liwdgSize liwdgSize;
typedef struct _liwdgSubimage liwdgSubimage;

struct _liwdgRect
{
	int x;
	int y;
	int width;
	int height;
};

struct _liwdgSize
{
	int width;
	int height;
};

struct _liwdgSubimage
{
	liimgTexture* texture;
	int x, y;
	int w[3];
	int h[3];
	int pad[4];
};

#endif

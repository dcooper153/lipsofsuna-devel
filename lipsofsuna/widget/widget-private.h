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

#ifndef __WIDGET_PRIVATE_H__
#define __WIDGET_PRIVATE_H__

#include <algorithm/lips-algorithm.h>
#include "widget-group.h"
#include "widget-types.h"

typedef struct _liwdgMenuItem liwdgMenuItem;
typedef struct _liwdgMenuProxy liwdgMenuProxy;

struct _liwdgMenuGroup
{
	void* userdata;
	struct
	{
		void (*call)(void*, int);
		void* data;
	} callback;
	struct
	{
		int count;
		liwdgMenuItem** array;
	} items;
};

struct _liwdgMenu
{
	liwdgGroup base;
	lialgList* groups;
	lialgList* proxies;
	lifntFont* font;
	unsigned int vertical : 1;
};

struct _liwdgMenuItem
{
	int id;
	char* icon;
	char* text;
	liwdgMenuGroup* group;
	struct
	{
		int count;
		liwdgMenuItem** array;
	} items;
};

struct _liwdgMenuProxy
{
	/*char* icon;*/
	lifntLayout* label;
	lialgList* items;
	lialgList* proxies;
};

#endif

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

#ifndef __WIDGET_PRIVATE_H__
#define __WIDGET_PRIVATE_H__

#include <lipsofsuna/algorithm.h>
#include "widget-group.h"
#include "widget-types.h"

struct _LIWdgMenu
{
	LIWdgGroup base;
	LIFntFont* font;
	void* userdata;
	unsigned int autohide : 1;
	unsigned int vertical : 1;
	struct
	{
		int count;
		LIWdgMenuItem** array;
	} items;
};

struct _LIWdgMenuItem
{
	int id;
	char* icon;
	char* text;
	LIFntLayout* label;
	struct
	{
		liwdgHandler call;
		void* data;
	} callback;
	struct
	{
		int count;
		LIWdgMenuItem** array;
	} items;
};

#endif

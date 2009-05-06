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

#ifndef __WIDGET_LOGIN_H__
#define __WIDGET_LOGIN_H__

#include <widget/lips-widget.h>

#define LIWDG_LOGIN(o) ((liwdgLogin*)(o))

typedef struct _liwdgLogin liwdgLogin;
struct _liwdgLogin
{
	liwdgGroup base;
	int n_servers;
	char** servers;
	liwdgWidget* group_entries;
	liwdgWidget* group_buttons;
	liwdgWidget* label_host;
	liwdgWidget* label_name;
	liwdgWidget* label_pass;
	liwdgWidget* entry_host;
	liwdgWidget* entry_name;
	liwdgWidget* entry_pass;
	liwdgWidget* button_login;
	liwdgWidget* button_import;
};

extern const liwdgWidgetClass liwdgLoginType;

liwdgWidget*
liwdg_login_new (liwdgManager* manager);

#endif

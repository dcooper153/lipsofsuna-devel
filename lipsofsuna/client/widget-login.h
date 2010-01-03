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

#ifndef __WIDGET_LOGIN_H__
#define __WIDGET_LOGIN_H__

#include <lipsofsuna/widget.h>

#define LIWDG_LOGIN(o) ((LIWdgLogin*)(o))

typedef struct _LIWdgLogin LIWdgLogin;
struct _LIWdgLogin
{
	LIWdgGroup base;
	int n_servers;
	char** servers;
	LIWdgWidget* group_entries;
	LIWdgWidget* group_buttons;
	LIWdgWidget* label_host;
	LIWdgWidget* label_name;
	LIWdgWidget* label_pass;
	LIWdgWidget* entry_host;
	LIWdgWidget* entry_name;
	LIWdgWidget* entry_pass;
	LIWdgWidget* button_login;
	LIWdgWidget* button_import;
};

extern const LIWdgClass licli_widget_login;

LIWdgWidget*
liwdg_login_new (LIWdgManager* manager);

#endif

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

#include <system/lips-system.h>
#include "client-window.h"
#include "widget-login.h"

static const void*
private_base ();

static int
private_init (liwdgLogin*   self,
              liwdgManager* manager);
static void
private_free (liwdgLogin* self);

static int
private_event (liwdgLogin* self,
               liwdgEvent* event);

static void
private_import (liwdgLogin* self);

static void
private_login (liwdgLogin* self);

static int
private_filter_servers (const char* dir,
                        const char* name);

const liwdgClass liwdgLoginType =
{
	LIWDG_BASE_DYNAMIC, private_base, "Login", sizeof (liwdgLogin),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/****************************************************************************/

liwdgWidget*
liwdg_login_new (liwdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdgLoginType);
}

int
liwdg_login_read_server_list (liwdgLogin* self)
{
#warning Login widget is broken.
#if 0
	int i;
	int j;
	int count = 0;
	int count0 = 0;
	int count1 = 0;
	const char* name0;
	const char* name1;
	char* path;
	char** servers = NULL;
	lisysDir* servers0 = NULL;
	lisysDir* servers1 = NULL;

	/* Read the system server list. */
	path = lisys_path_concat (paths.global_data, "server", NULL);
	if (path == NULL)
		goto error;
	servers0 = lisys_dir_open (path);
	if (servers0 != NULL)
	{
		lisys_dir_set_filter (servers0, private_filter_servers);
		lisys_dir_set_sorter (servers0, LISYS_DIR_SORTER_ALPHA);
		lisys_dir_scan (servers0);
		count1 = lisys_dir_get_count (servers0);
	}
	free (path);

	/* Read the local server list. */
	path = lisys_path_concat (paths.local_data, "servers", NULL);
	if (path == NULL)
		goto error;
	servers1 = lisys_dir_open (path);
	if (servers1 != NULL)
	{
		lisys_dir_set_filter (servers1, private_filter_servers);
		lisys_dir_set_sorter (servers1, LISYS_DIR_SORTER_ALPHA);
		lisys_dir_scan (servers1);
		count1 = lisys_dir_get_count (servers1);
	}
	free (path);

	/* Merge the server lists. */
	if (count0 + count1)
	{
		servers = calloc (count0 + count1, sizeof (char*));
		if (servers == NULL)
			goto error;
		for (i = j = 0 ; i < count0 && j < count1 ; )
		{
			name0 = lisys_dir_get_name (servers0, i);
			name1 = lisys_dir_get_name (servers1, j);
			switch (strcmp (name0, name1))
			{
				case -1:
					i++;
					servers[count] = strdup (name0);
					if (servers[count++] == NULL)
						goto error;
					break;
				case 0:
					i++;
					j++;
					servers[count] = strdup (name1);
					if (servers[count++] == NULL)
						goto error;
					break;
				case 1:
					j++;
					servers[count] = strdup (name1);
					if (servers[count++] == NULL)
						goto error;
					break;
			}
		}
		while (i < count0)
		{
			servers[count] = strdup (lisys_dir_get_name (servers0, i++));
			if (servers[count++] == NULL)
				goto error;
		}
		while (j < count1)
		{
			servers[count] = strdup (lisys_dir_get_name (servers1, i++));
			if (servers[count++] == NULL)
				goto error;
		}
		if (servers0 != NULL)
			lisys_dir_free (servers0);
		if (servers1 != NULL)
			lisys_dir_free (servers1);
	}

	/* Replace the old server list. */
	if (self->servers != NULL)
	{
		for (i = 0 ; i < self->n_servers ; i++)
			free (self->servers[i]);
		free (self->servers);
	}
	self->n_servers = count;
	self->servers = servers;
	return 1;

error:
	for (i = 0 ; i < count ; i++)
		free (servers[i]);
	free (servers);
	if (servers0 != NULL)
		lisys_dir_free (servers0);
	if (servers1 != NULL)
		lisys_dir_free (servers1);
	return 0;
#endif

	return 0;
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgGroupType;
}

static int
private_init (liwdgLogin*   self,
              liwdgManager* manager)
{
	int i;
	int w;
	int h;
	liwdgWidget* widgets[] =
	{
		liwdg_group_new_with_size (manager, 2, 3),
		liwdg_group_new_with_size (manager, 2, 1),
		liwdg_label_new (manager),
		liwdg_label_new (manager),
		liwdg_label_new (manager),
		liwdg_entry_new (manager),
		liwdg_entry_new (manager),
		liwdg_entry_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
	};

	/* Check memory. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 1, 3))
		goto error;
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (liwdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			goto error;
	}

	/* Assign widgets. */
	self->group_entries = widgets[0];
	self->group_buttons = widgets[1];
	self->label_host = widgets[2];
	self->label_name = widgets[3];
	self->label_pass = widgets[4];
	self->entry_host = widgets[5];
	self->entry_name = widgets[6];
	self->entry_pass = widgets[7];
	self->button_import = widgets[8];
	self->button_login = widgets[9];

	/* Load the server list. */
	liwdg_login_read_server_list (self);

	/* Pack entry group. */
	liwdg_label_set_text (LIWDG_LABEL (self->label_host), "Server:");
	liwdg_label_set_text (LIWDG_LABEL (self->label_name), "Account:");
	liwdg_label_set_text (LIWDG_LABEL (self->label_pass), "Password:");
	liwdg_entry_set_secret (LIWDG_ENTRY (self->entry_pass), 1);
	if (self->n_servers > 0)
		liwdg_entry_set_text (LIWDG_ENTRY (self->entry_host), self->servers[0]);
	liwdg_group_set_spacings (LIWDG_GROUP (self->group_entries), 5, 0);
	liwdg_group_set_child (LIWDG_GROUP (self->group_entries), 0, 2, self->label_host);
	liwdg_group_set_child (LIWDG_GROUP (self->group_entries), 0, 1, self->label_name);
	liwdg_group_set_child (LIWDG_GROUP (self->group_entries), 0, 0, self->label_pass);
	liwdg_group_set_child (LIWDG_GROUP (self->group_entries), 1, 2, self->entry_host);
	liwdg_group_set_child (LIWDG_GROUP (self->group_entries), 1, 1, self->entry_name);
	liwdg_group_set_child (LIWDG_GROUP (self->group_entries), 1, 0, self->entry_pass);

	/* Pack button group. */
	liwdg_button_set_text (LIWDG_BUTTON (self->button_import), "Import");
	liwdg_widget_insert_callback (self->button_import, LIWDG_CALLBACK_PRESSED, 0, private_import, self, NULL);
	liwdg_button_set_text (LIWDG_BUTTON (self->button_login), "Login");
	liwdg_widget_insert_callback (self->button_login, LIWDG_CALLBACK_PRESSED, 0, private_login, self, NULL);
	liwdg_group_set_spacings (LIWDG_GROUP (self->group_buttons), 5, 0);
	liwdg_group_set_child (LIWDG_GROUP (self->group_buttons), 0, 0, self->button_import);
	liwdg_group_set_child (LIWDG_GROUP (self->group_buttons), 1, 0, self->button_login);

	/* Pack self. */
	liwdg_manager_get_size (LIWDG_WIDGET (self)->manager, &w, &h);
	liwdg_widget_set_allocation (LIWDG_WIDGET (self), 0, 0, w, h);
	liwdg_group_set_row_expand (LIWDG_GROUP (self), 2, 1);
	liwdg_group_set_margins (LIWDG_GROUP (self), 10, 10, 10, 10);
	liwdg_group_set_spacings (LIWDG_GROUP (self), 5, 5);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 1, self->group_entries);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, self->group_buttons);
	liwdg_manager_set_focus_keyboard (manager, self->entry_name);
	return 1;

error:
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (liwdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			liwdg_widget_free (widgets[i]);
	}
	return 0;
}

static void
private_free (liwdgLogin* self)
{
	int i;

	/* Free the server list. */
	if (self->servers != NULL)
	{
		for (i = 0 ; i < self->n_servers ; i++)
			free (self->servers[i]);
		free (self->servers);
	}
}

static int
private_event (liwdgLogin* self,
               liwdgEvent* event)
{
	return liwdgGroupType.event (LIWDG_WIDGET (self), event);
}

static void
private_import (liwdgLogin* self)
{
	printf ("FIXME: Import not implemented yet!\n");
}

static void
private_login (liwdgLogin* self)
{
#if 0
	li_init_game (
		liwdg_entry_get_text (LIWDG_ENTRY (self->entry_host)),
		liwdg_entry_get_text (LIWDG_ENTRY (self->entry_name)),
		liwdg_entry_get_text (LIWDG_ENTRY (self->entry_pass)));
#endif
}

static int
private_filter_servers (const char* dir,
                        const char* name)
{
	return LISYS_DIR_FILTER_VISIBLE (dir, name) &&
	       LISYS_DIR_FILTER_DIRS (dir, name);
}

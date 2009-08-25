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
 * \addtogroup liwdgMenuGroup MenuGroup
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "widget-menugroup.h"
#include "widget-private.h"

static liwdgMenuItem*
private_item_create (liwdgMenuGroup* self,
                     liwdgMenuItem*  parent,
                     const char*     text);

static void
private_item_free (liwdgMenuGroup* self,
                   liwdgMenuItem*  item);

static liwdgMenuItem*
private_item_find (liwdgMenuGroup* self,
                   liwdgMenuItem*  parent,
                   const char*     text);

/*****************************************************************************/

liwdgMenuGroup*
liwdg_menu_group_new (const char* markup)
{
	int id;
	char* icon;
	char* name;
	liarcReader* reader;
	liwdgMenuGroup* self;
	liwdgMenuItem* item;
	liwdgMenuItem* parent;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liwdgMenuGroup));
	if (self == NULL)
		return NULL;

	/* Parse markup. */
	reader = liarc_reader_new (markup, strlen (markup));
	liarc_reader_skip_chars (reader, " \t\n");
	while (!liarc_reader_check_end (reader))
	{
		icon = NULL;
		name = NULL;
		item = NULL;
		parent = NULL;

		/* Read id and icon. */
		if (!liarc_reader_get_text_int (reader, &id) ||
		    !liarc_reader_skip_chars (reader, " \t") ||
		    !liarc_reader_get_text (reader, " \t", &icon))
			goto error;

		/* Read path. */
		while (!liarc_reader_check_end (reader) && reader->buffer[reader->pos - 1] != '\n')
		{
			/* Read path component. */
			if (!liarc_reader_get_text (reader, "|\n", &name))
			{
				lisys_free (icon);
				goto error;
			}

			/* Find or create item. */
			parent = item;
			item = private_item_find (self, parent, name);
			if (item == NULL)
			{
				item = private_item_create (self, parent, name);
				if (item == NULL)
				{
					lisys_free (icon);
					lisys_free (name);
					goto error;
				}
			}
			lisys_free (name);
		}

		/* Skip whitespace. */
		liarc_reader_skip_chars (reader, " \t\n");

		/* Set id and icon. */
		if (item == NULL)
		{
			lisys_error_set (EINVAL, "invalid menu markup");
			goto error;
		}
		item->id = id;
		item->icon = icon;
	}
	liarc_reader_free (reader);

	return self;

error:
	liarc_reader_free (reader);
	liwdg_menu_group_free (self);
	return NULL;
}

void
liwdg_menu_group_free (liwdgMenuGroup* self)
{
	int i;

	for (i = 0 ; i < self->items.count ; i++)
		private_item_free (self, self->items.array[i]);
	lisys_free (self->items.array);
	lisys_free (self);
}

void
liwdg_menu_group_set_callback (liwdgMenuGroup* self,
                               liwdgHandler    call,
                               void*           data)
{
	self->callback.call = (void*) call;
	self->callback.data = data;
}

void*
liwdg_menu_group_get_userdata (liwdgMenuGroup* self)
{
	return self->userdata;
}

void
liwdg_menu_group_set_userdata (liwdgMenuGroup* self,
                               void*           value)
{
	self->userdata = value;
}

/*****************************************************************************/

static liwdgMenuItem*
private_item_create (liwdgMenuGroup* self,
                     liwdgMenuItem*  parent,
                     const char*     text)
{
	liwdgMenuItem* item;

	/* Allocate item. */
	item = lisys_calloc (1, sizeof (liwdgMenuItem));
	if (item == NULL)
		return 0;
	item->id = -1;
	item->group = self;
	item->text = listr_dup (text);
	if (item->text == NULL)
	{
		lisys_free (item);
		return NULL;
	}

	/* Append to item list. */
	if (parent != NULL)
	{
		if (!lialg_array_append (&parent->items, &item))
		{
			lisys_free (item->text);
			lisys_free (item);
			return NULL;
		}
	}
	else
	{
		if (!lialg_array_append (&self->items, &item))
		{
			lisys_free (item->text);
			lisys_free (item);
			return NULL;
		}
	}

	return item;
}

static void
private_item_free (liwdgMenuGroup* self,
                   liwdgMenuItem*  item)
{
	int i;

	for (i = 0 ; i < item->items.count ; i++)
		private_item_free (self, item->items.array[i]);
	lisys_free (item->items.array);
	lisys_free (item->icon);
	lisys_free (item->text);
	lisys_free (item);
}

static liwdgMenuItem*
private_item_find (liwdgMenuGroup* self,
                   liwdgMenuItem*  parent,
                   const char*     text)
{
	int i;

	if (parent != NULL)
	{
		for (i = 0 ; i < parent->items.count ; i++)
		{
			if (!strcmp (parent->items.array[i]->text, text))
				return parent->items.array[i];
		}
	}
	else
	{
		for (i = 0 ; i < self->items.count ; i++)
		{
			if (!strcmp (self->items.array[i]->text, text))
				return self->items.array[i];
		}
	}

	return NULL;
}

/** @} */
/** @} */

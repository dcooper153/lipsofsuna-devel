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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliChat Chat          
 * @{
 */

#include <class/lips-class.h>
#include <client/lips-client.h>
#include <network/lips-network.h>
#include "ext-entry.h"

static const void*
private_base ();

static int
private_init (liextChatEntry* self,
              liwdgManager*   manager);

static void
private_free (liextChatEntry* self);

static int
private_event (liextChatEntry* self,
               liwdgEvent*     event);

static int
private_send (liextChatEntry* self);

/****************************************************************************/

const liwdgWidgetClass liextChatEntryType =
{
	LI_CLASS_BASE_DYNAMIC, private_base, "ChatEntry", sizeof (liextChatEntry),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

liwdgWidget*
liext_chat_entry_new (liwdgManager* manager,
                      liextModule*  module)
{
	liwdgWidget* self;
	
	self = li_instance_new (&liextChatEntryType, manager);
	if (self == NULL)
		return self;
	LIEXT_WIDGET_CHAT_ENTRY (self)->module = module;

	return self;
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgGroupType;
}

static int
private_init (liextChatEntry* self,
              liwdgManager*   manager)
{
	/* Allocate widgets. */
	self->label = liwdg_label_new (manager);
	self->button = liwdg_button_new (manager);
	self->entry = liwdg_entry_new (manager);
	if (self->button == NULL || self->entry == NULL)
		goto error;

	/* Pack widgets. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 3, 1))
		goto error;
	liwdg_label_set_text (LIWDG_LABEL (self->label), ">");
	liwdg_button_set_text (LIWDG_BUTTON (self->button), "Send");
	liwdg_widget_insert_callback (self->button, LIWDG_CALLBACK_PRESSED, 0, private_send, self);
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 1, 1);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, self->label);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 0, self->entry);
	liwdg_group_set_child (LIWDG_GROUP (self), 2, 0, self->button);

	return 1;

error:
	if (self->label != NULL)
		liwdg_widget_free (self->label);
	if (self->button != NULL)
		liwdg_widget_free (self->button);
	if (self->entry != NULL)
		liwdg_widget_free (self->entry);
	return 0;
}

static void
private_free (liextChatEntry* self)
{
}

static int
private_event (liextChatEntry* self,
               liwdgEvent*     event)
{
	return liwdgGroupType.event (LIWDG_WIDGET (self), event);
}

static int
private_send (liextChatEntry* self)
{
	const char* text;

	/* Get message. */
	text = liwdg_entry_get_text (LIWDG_ENTRY (self->entry));
	if (!strlen (text))
		return 1;

	/* Send packet. */
	if (liext_module_send (self->module, text))
		liwdg_entry_clear (LIWDG_ENTRY (self->entry));

	return 1;
}

/** @} */
/** @} */
/** @} */

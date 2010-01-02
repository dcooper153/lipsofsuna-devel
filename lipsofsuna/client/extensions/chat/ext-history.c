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

/**
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliChat Chat          
 * @{
 */

#include <client/lips-client.h>
#include "ext-history.h"

#define LIEXT_CHAT_HISTORY_DEFAULT_LINES 10

static const void*
private_base ();

static int
private_init (liextChatHistory* self,
              liwdgManager* manager);

static void
private_free (liextChatHistory* self);

static int
private_event (liextChatHistory* self,
               liwdgEvent*       event);

static void
private_rebuild (liextChatHistory* self);

/****************************************************************************/

const liwdgClass liextChatHistoryType =
{
	LIWDG_BASE_DYNAMIC, private_base, "ChatHistory", sizeof (liextChatHistory),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/**
 * \brief Creates a new chat history widget.
 *
 * \param manager Widget manager.
 * \param client Client.
 * \return New widget or NULL.
 */
liwdgWidget*
liext_chat_history_new (liwdgManager* manager,
                        licliClient*  client)
{
	liwdgWidget* self;

	/* Allocate self. */
	self = liwdg_widget_new (manager, &liextChatHistoryType);
	if (self == NULL)
		return NULL;
	LIEXT_WIDGET_CHAT_HISTORY (self)->client = client;

	return self;
}

/**
 * \brief Appends a message to the history widget.
 *
 * \param self Chat history.
 * \param message Message.
 * \return Nonzero on success.
 */
int
liext_chat_history_append (liextChatHistory* self,
                           const char*       message)
{
	int i;
	char* msg;

	msg = listr_dup (message);
	if (msg == NULL)
		return 0;

	/* Scroll lines. */
	lisys_free (self->lines.array[0]);
	for (i = 0 ; i < self->lines.count - 1 ; i++)
		self->lines.array[i] = self->lines.array[i + 1];

	/* Insert new line to bottom. */
	self->lines.array[self->lines.count - 1] = msg;
	private_rebuild (self);

	return 1;
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgLabelType;
}

static int
private_init (liextChatHistory* self,
              liwdgManager*     manager)
{
	/* Allocate lines. */
	self->lines.count = LIEXT_CHAT_HISTORY_DEFAULT_LINES;
	self->lines.array = lisys_calloc (1, self->lines.count * sizeof (char*));
	if (self->lines.array == NULL)
		return 0;
	liwdg_label_set_valign (LIWDG_LABEL (self), 1.0f);

	/* Format text. */
	private_rebuild (self);
	return 1;
}

static void
private_free (liextChatHistory* self)
{
	int i;

	/* Free lines. */
	if (self->lines.array != NULL)
	{
		for (i = 0 ; i < self->lines.count ; i++)
			lisys_free (self->lines.array[i]);
		lisys_free (self->lines.array);
	}
}

static int
private_event (liextChatHistory* self,
               liwdgEvent*       event)
{
	return liwdgLabelType.event (LIWDG_WIDGET (self), event);
}

static void
private_rebuild (liextChatHistory* self)
{
	int i;
	const char* line;
	liarcWriter* writer;

	writer = liarc_writer_new ();
	if (writer == NULL)
		return;

	for (i = 0 ; i < self->lines.count ; i++)
	{
		line = self->lines.array[i];
		if (line != NULL)
			liarc_writer_append_string (writer, line);
		liarc_writer_append_string (writer, "\n");
	}
	liarc_writer_append_nul (writer);

	if (!writer->error)
		liwdg_label_set_text (LIWDG_LABEL (self), liarc_writer_get_buffer (writer));
	liarc_writer_free (writer);
}

/** @} */
/** @} */
/** @} */

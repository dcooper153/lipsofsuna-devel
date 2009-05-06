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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvCallbacks Callbacks
 * @{
 */

#include <network/lips-network.h>
#include "server-callbacks.h"
#include "server-observer.h"
#include "server-script.h"

static int
private_client_packet_action (lisrvClient* self,
                              liReader*    reader)
{
	uint8_t action;

	if (!li_reader_get_uint8 (reader, &action))
		return 0;
	lisrv_server_event (self->server, LISRV_EVENT_TYPE_ACTION,
		"*object", LICOM_SCRIPT_OBJECT, self->object,
		"action", LISCR_TYPE_INT, (int) action, NULL);
	return 1;
}

static int
private_client_packet_chat (lisrvClient* self,
                            liReader*    reader)
{
	uint8_t flags;
	char* message = NULL;

	/* Get message. */
	if (!li_reader_get_uint8 (reader, &flags) ||
	    !li_reader_get_text (reader, "", &message) ||
	    !li_reader_check_end (reader) ||
	    !li_string_utf8_get_valid (message))
	{
		free (message);
		return 0;
	}

	/* Get sector. */
	if (self->object == NULL || self->object->sector == NULL)
	{
		free (message);
		return 1;
	}

	/* Emit an event to self. */
	lisrv_server_event (self->server, LISRV_EVENT_TYPE_MESSAGE,
		"*object", LICOM_SCRIPT_OBJECT, self->object,
		"message", LISCR_TYPE_STRING, message, NULL);
	free (message);
	return 1;
}

static int
private_client_packet_move (lisrvClient* self,
                            liReader*    reader)
{
	int8_t x;
	int8_t y;
	int8_t z;
	int8_t w;
	uint8_t flags;
	limatQuaternion tmp;
	limatTransform transform;

	/* Parse the packet. */
	if (!li_reader_get_uint8 (reader, &flags) || (flags & ~LI_CONTROL_MASK) ||
	    !li_reader_get_int8 (reader, &x) ||
	    !li_reader_get_int8 (reader, &y) ||
	    !li_reader_get_int8 (reader, &z) ||
	    !li_reader_get_int8 (reader, &w) ||
	    !li_reader_check_end (reader))
		return 0;
	if (self->object != NULL && lisrv_object_get_valid (self->object))
	{
		/* FIXME: Should be included to the event. */
		lieng_object_get_transform (self->object, &transform);
		tmp = limat_quaternion_init (x / 127.0f, y / 127.0f, z / 127.0f, w / 127.0f);
		transform.rotation = limat_quaternion_normalize (tmp);
		lieng_object_set_transform (self->object, &transform);
	}
	lisrv_server_event (self->server, LISRV_EVENT_TYPE_CONTROL,
		"*object", LICOM_SCRIPT_OBJECT, self->object,
		"controls", LISCR_TYPE_INT, flags, NULL);

	return 1;
}

/****/

static int
private_client_client_login (lisrvServer* server,
                             liengObject* object,
                             const char*  name,
                             const char*  pass)
{
#warning No accounts
#if 0
	char* path;
	licfgAccount* account;

	/* Contruct the account path. */
	path = lisys_path_concat (self->paths->server_state, "accounts", message->NEW_USER.name, NULL);
	if (path == NULL)
	{
		grapple_server_disconnect_client (self->network.server, message->NEW_USER.id);
		free (pass);
		return 1;
	}

	/* Parse the account file. */
	account = licfg_account_new (path);
	free (path);
	if (account == NULL)
	{
		if (lisys_error_peek () != EIO)
		{
			grapple_server_disconnect_client (self->network.server, message->NEW_USER.id);
			free (pass);
			return 1;
		}
		if (!self->config.server->enable_create_account)
		{
			grapple_server_disconnect_client (self->network.server, message->NEW_USER.id);
			free (pass);
			return 1;
		}
	}

	/* Set credentials. */
	lisrv_client_set_account (client, message->NEW_USER.name);
	if (pass != NULL)
		lisrv_client_set_password (client, pass);
#endif

	return 1;
}

static int
private_client_client_packet (lisrvServer* server,
                              lisrvClient* client,
                              liReader*    packet)
{
	int ret = 1;

	switch (((uint8_t*) packet->buffer)[0])
	{
		case LI_CLIENT_COMMAND_ACTION:
			ret = private_client_packet_action (client, packet);
			break;
		case LI_CLIENT_COMMAND_CHAT:
			ret = private_client_packet_chat (client, packet);
			break;
		case LI_CLIENT_COMMAND_MOVE:
			ret = private_client_packet_move (client, packet);
			break;
	}

	return ret;
}

static int
private_client_vision_hide (lisrvServer* server,
                            liengObject* object,
                            liengObject* target)
{
	liarcWriter* writer;

	writer = liarc_writer_new_packet (LINET_SERVER_PACKET_OBJECT_DESTROY);
	if (writer != NULL)
	{
		liarc_writer_append_uint32 (writer, target->id);
		lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}

	return 1;
}

static int
private_client_vision_show (lisrvServer* server,
                            liengObject* object,
                            liengObject* target)
{
	int engflags;
	int netflags;
	lialgU32dicIter iter;
	liarcWriter* writer;
	limatTransform transform;
	limatVector velocity;
	lisrvAniminfo* animation;

	/* Create standard vision message. */
	writer = liarc_writer_new_packet (LINET_SERVER_PACKET_OBJECT_CREATE);
	if (writer != NULL)
	{
		/* Translate flags. */
		netflags = 0;
		engflags = lieng_object_get_flags (target);
		if (engflags & LIENG_OBJECT_FLAG_DYNAMIC)
			netflags |= LINET_OBJECT_FLAG_DYNAMIC;

		/* Send creation packet. */
		lieng_object_get_transform (target, &transform);
		lieng_object_get_velocity (target, &velocity);
		liarc_writer_append_uint32 (writer, target->id);
		liarc_writer_append_uint16 (writer, lieng_object_get_model_code (target));
		liarc_writer_append_uint8 (writer, netflags);
		liarc_writer_append_int8 (writer, (int8_t)(127 * transform.rotation.x));
		liarc_writer_append_int8 (writer, (int8_t)(127 * transform.rotation.y));
		liarc_writer_append_int8 (writer, (int8_t)(127 * transform.rotation.z));
		liarc_writer_append_int8 (writer, (int8_t)(127 * transform.rotation.w));
		liarc_writer_append_float (writer, velocity.x);
		liarc_writer_append_float (writer, velocity.y);
		liarc_writer_append_float (writer, velocity.z);
		liarc_writer_append_float (writer, transform.position.x);
		liarc_writer_append_float (writer, transform.position.y);
		liarc_writer_append_float (writer, transform.position.z);
		liarc_writer_append_uint8 (writer, LISRV_OBJECT (target)->animations->size);
		LI_FOREACH_U32DIC (iter, LISRV_OBJECT (target)->animations)
		{
			animation = iter.value;
			liarc_writer_append_uint16 (writer, animation->animation->id);
			liarc_writer_append_uint8 (writer, animation->channel);
			liarc_writer_append_uint8 (writer, animation->permanent);
			liarc_writer_append_float (writer, animation->priority);
		}
		lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}

	return 1;
}

int lisrv_server_init_callbacks_client (lisrvServer* server)
{
	lieng_engine_call_insert (server->engine, LISRV_CALLBACK_CLIENT_LOGIN, 0, private_client_client_login, server);
	lieng_engine_call_insert (server->engine, LISRV_CALLBACK_CLIENT_PACKET, 0, private_client_client_packet, server);
	lieng_engine_call_insert (server->engine, LISRV_CALLBACK_VISION_HIDE, 0, private_client_vision_hide, server);
	lieng_engine_call_insert (server->engine, LISRV_CALLBACK_VISION_SHOW, 0, private_client_vision_show, server);
	return 1;
}

/*****************************************************************************/

static int
private_event_client_login (lisrvServer* server,
                            liengObject* object,
                            const char*  name,
                            const char*  pass)
{
	lisrv_server_event (server, LISRV_EVENT_TYPE_LOGIN,
		"*object", LICOM_SCRIPT_OBJECT, object, NULL);
	return 1;
}

static int
private_event_client_logout (lisrvServer* server,
                             lisrvObject* object)
{
	lisrv_server_event (server, LISRV_EVENT_TYPE_LOGOUT,
		"*object", LICOM_SCRIPT_OBJECT, object, NULL);
	return 1;
}

static int
private_event_client_packet (lisrvServer* server,
                             lisrvClient* client,
                             liReader*    packet)
{
	lisrv_server_event (server, LISRV_EVENT_TYPE_PACKET,
		"*object", LICOM_SCRIPT_OBJECT, client->object,
		"message", LISCR_TYPE_INT, (int)(((uint8_t*) packet->buffer)[0]),
		"packet", LICOM_SCRIPT_PACKET, packet, NULL);
	return 1;
}

static int
private_event_object_animation (lisrvServer*   server,
                                liengObject*   object,
                                lisrvAniminfo* info)
{
#warning animation events disabled.
#if 0
	lisrv_server_event (server, LISRV_EVENT_TYPE_OBJECT_ANIMATION,
		"*object", LICOM_SCRIPT_OBJECT, object,
		"animation", LISCR_TYPE_STRING, animation->name, NULL);
#endif
	return 1;
}

static int
private_event_object_effect (lisrvServer* server,
                             liengObject* object,
                             licfgEffect* effect,
                             int          flags)
{
	lisrv_server_event (server, LISRV_EVENT_TYPE_EFFECT,
		"*object", LICOM_SCRIPT_OBJECT, object,
		"effect", LISCR_TYPE_STRING, effect->name,
		"flags", LISCR_TYPE_INT, flags, NULL);
	return 1;
}

static int
private_event_object_motion (lisrvServer* server,
                             liengObject* object)
{
	lisrv_server_event (server, LISRV_EVENT_TYPE_SIMULATE,
		"*object", LICOM_SCRIPT_OBJECT, object, NULL);
	return 1;
}

static int
private_event_object_speech (lisrvServer* server,
                             liengObject* object,
                             const char*  message)
{
	lisrv_server_event (server, LISRV_EVENT_TYPE_SPEECH,
		"*object", LICOM_SCRIPT_OBJECT, object,
		"message", LISCR_TYPE_STRING, message, NULL);
	return 1;
}

static int
private_event_object_visibility (lisrvServer* server,
                                 liengObject* object,
                                 int          visible)
{
	lisrv_server_event (server, LISRV_EVENT_TYPE_VISIBILITY,
		"*object", LICOM_SCRIPT_OBJECT, object,
		"visible", LISCR_TYPE_BOOLEAN, visible, NULL);
	return 1;
}

int lisrv_server_init_callbacks_event (lisrvServer* server)
{
	lieng_engine_call_insert (server->engine, LISRV_CALLBACK_CLIENT_LOGIN, 0, private_event_client_login, server);
	lieng_engine_call_insert (server->engine, LISRV_CALLBACK_CLIENT_LOGOUT, 0, private_event_client_logout, server);
	lieng_engine_call_insert (server->engine, LISRV_CALLBACK_CLIENT_PACKET, 0, private_event_client_packet, server);
	lieng_engine_call_insert (server->engine, LISRV_CALLBACK_OBJECT_ANIMATION, 0, private_event_object_animation, server);
	lieng_engine_call_insert (server->engine, LISRV_CALLBACK_OBJECT_EFFECT, 0, private_event_object_effect, server);
	lieng_engine_call_insert (server->engine, LISRV_CALLBACK_OBJECT_MOTION, 0, private_event_object_motion, server);
	lieng_engine_call_insert (server->engine, LISRV_CALLBACK_OBJECT_SPEECH, 0, private_event_object_speech, server);
	lieng_engine_call_insert (server->engine, LISRV_CALLBACK_OBJECT_VISIBILITY, 0, private_event_object_visibility, server);
	return 1;
}

/** @} */
/** @} */

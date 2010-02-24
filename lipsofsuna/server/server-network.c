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
 * \addtogroup liser Server
 * @{
 * \addtogroup LISerNetwork Network
 * @{
 */

#include <lipsofsuna/network.h>
#include "server-network.h"

static int
private_init (LISerNetwork* self,
              int           udp,
              int           port);

static int
private_accept (grapple_user  user,
                LISerNetwork* self);

static int
private_login (const char*   login,
               const char*   password,
               LISerNetwork* self);

static int
private_connect (LISerNetwork*    self,
                 grapple_message* message);

static int
private_disconnect (LISerNetwork*    self,
                    grapple_message* message);

static int
private_message (LISerNetwork*    self,
                 grapple_message* message);

static int
private_rename (LISerNetwork*    self,
                grapple_message* message);

static int
private_update (LISerNetwork* self,
                float         secs);

/*****************************************************************************/

LISerNetwork*
liser_network_new (LISerServer* server,
                   int          udp,
                   int          port)
{
	LISerNetwork* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LISerNetwork));
	if (self == NULL)
		return NULL;
	self->server = server;

	/* Connect callbacks. */
	if (!lical_callbacks_insert (server->callbacks, server->engine, "tick", 0, private_update, self, self->calls))
	{
		lisys_free (self);
		return NULL;
	}

	/* Initialize self. */
	pthread_mutex_init (&self->mutex, NULL);
	if (!private_init (self, udp, port))
	{
		liser_network_free (self);
		return NULL;
	}

	return self;
}

void
liser_network_free (LISerNetwork* self)
{
	LIAlgU32dicIter iter0;
	LIAlgStrdicIter iter1;

	if (self->clients != NULL)
	{
		LIALG_U32DIC_FOREACH (iter0, self->clients)
			liser_client_free (iter0.value);
		lialg_u32dic_free (self->clients);
	}
	if (self->passwords != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->passwords)
			lisys_free (iter1.value);
		lialg_strdic_free (self->passwords);
	}
	if (self->resources != NULL)
		liarc_writer_free (self->resources);
	if (self->server != 0)
		grapple_server_destroy (self->socket);
	pthread_mutex_destroy (&self->mutex);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

void
liser_network_update (LISerNetwork* self,
                      float         secs)
{
	LIAlgU32dicIter iter;
	LISerClient* client;
	grapple_message* message;

	/* Handle messages. */
	if (grapple_server_messages_waiting (self->socket))
	{
		message = grapple_server_message_pull (self->socket);
		switch (message->type)
		{
			case GRAPPLE_MSG_NEW_USER:
				private_connect (self, message);
				break;
			case GRAPPLE_MSG_USER_NAME:
				private_rename (self, message);
				break;
			case GRAPPLE_MSG_USER_MSG:
				private_message (self, message);
				break;
			case GRAPPLE_MSG_USER_DISCONNECTED:
				private_disconnect (self, message);
				break;
			default:
				printf ("WARNING: Unknown message from client.");
				break;
		}
		grapple_message_dispose (message);
	}

	/* Prevent sectors from unloading. */
	LIALG_U32DIC_FOREACH (iter, self->clients)
	{
		client = iter.value;
		if (client->object != NULL)
			lieng_object_refresh (client->object, client->radius + 1.0f);
	}
}

/**
 * \brief Find a client by network user.
 *
 * \param self Network.
 * \param user Network user.
 * \return Client or NULL.
 */
LISerClient*
liser_network_find_client (LISerNetwork* self,
                           grapple_user  user)
{
	return lialg_u32dic_find (self->clients, user);
}

/*****************************************************************************/

static int
private_init (LISerNetwork* self,
              int           udp,
              int           port)
{
	int i;
	LIEngAnimation* anim;
	LIEngModel* model;
	LIEngSample* sample;
	grapple_error error;

	/* Allocate client list. */
	self->clients = lialg_u32dic_new ();
	if (self->clients == NULL)
		return 0;
	self->passwords = lialg_strdic_new ();
	if (self->passwords == NULL)
		return 0;

	/* Build resource packet. */
	/* TODO: Should use compression. */
	self->resources = liarc_writer_new_packet (LINET_SERVER_PACKET_RESOURCES);
	if (self->resources == NULL)
		return 0;
	liarc_writer_append_uint32 (self->resources, self->server->engine->resources->animations.count);
	liarc_writer_append_uint32 (self->resources, self->server->engine->resources->models.count);
	liarc_writer_append_uint32 (self->resources, self->server->engine->resources->samples.count);
	for (i = 0 ; i < self->server->engine->resources->animations.count ; i++)
	{
		anim = lieng_resources_find_animation_by_code (self->server->engine->resources, i);
		assert (anim != NULL);
		liarc_writer_append_string (self->resources, anim->name);
		liarc_writer_append_nul (self->resources);
	}
	for (i = 0 ; i < self->server->engine->resources->models.count ; i++)
	{
		model = lieng_resources_find_model_by_code (self->server->engine->resources, i);
		assert (model != NULL);
		liarc_writer_append_string (self->resources, model->name);
		liarc_writer_append_nul (self->resources);
	}
	for (i = 0 ; i < self->server->engine->resources->samples.count ; i++)
	{
		sample = lieng_resources_find_sample_by_code (self->server->engine->resources, i);
		assert (sample != NULL);
		liarc_writer_append_string (self->resources, sample->name);
		liarc_writer_append_nul (self->resources);
	}

	/* Initialize socket. */
	self->socket = grapple_server_init ("Lips of Suna", LINET_PROTOCOL_VERSION);
	grapple_server_notified_set (self->socket, GRAPPLE_NOTIFY_STATE_OFF);
	grapple_server_port_set (self->socket, port);
	grapple_server_protocol_set (self->socket, udp? GRAPPLE_PROTOCOL_UDP : GRAPPLE_PROTOCOL_TCP);
	grapple_server_session_set (self->socket, "");
	grapple_server_namepolicy_set (self->socket, GRAPPLE_NAMEPOLICY_REQUIRED);
	grapple_server_connectionhandler_set (self->socket, (grapple_connection_callback) private_accept, self);
	grapple_server_passwordhandler_set (self->socket, (grapple_password_callback) private_login, self);
	if (grapple_server_start (self->socket) != GRAPPLE_OK)
	{
		error = grapple_server_error_get (self->socket);
		lisys_error_set (EINVAL, "host: %s", grapple_error_text (error));
		return 0;
	}

	return 1;
}

/**
 * \brief Handles newly accepted connections.
 *
 * This function is called by Grapple from one of its worker threads.
 * The main game thread is running in the background so we need to lock
 * any data we use.
 *
 * \param user Grapple user.
 * \param self Network.
 * \return Nonzero if the user is allowed to enter.
 */
static int
private_accept (grapple_user  user,
                LISerNetwork* self)
{
	int ret = 1;
	char* address;

	/* Get client address. */
	address = grapple_server_client_address_get (self->socket, user);
	if (address == NULL)
		return 0;

	/* TODO: Check if banned. */
	lisys_free (address);
	return ret;
}

/**
 * \brief Handles users who send in their login request.
 *
 * This function is called by Grapple from one of its worker threads.
 * The main game thread is running in the background so we need to lock
 * any data we use.
 *
 * \param login Login name.
 * \param password User password.
 * \param self Network.
 * \return Nonzero if the user is allowed to enter.
 */
static int
private_login (const char*   login,
               const char*   password,
               LISerNetwork* self)
{
	int ret = 1;

	/* Sanity checks. */
	if (login == NULL || password == NULL || !strlen (login) || !strlen (password))
		return 0;

	/* TODO: Check for account. */

	return ret;
}

static int
private_connect (LISerNetwork*    self,
                 grapple_message* message)
{
	int flags;
	char* pass;
	LIEngObject* object;
	LISerClient* client;

	/* Get temporarily stored password. */
	pthread_mutex_lock (&self->mutex);
	pass = lialg_strdic_find (self->passwords, message->NEW_USER.name);
	if (pass != NULL)
		lialg_strdic_remove (self->passwords, message->NEW_USER.name);
	pthread_mutex_unlock (&self->mutex);

	/* Create object. */
	liscr_script_set_gc (self->server->script, 0);
	object = lieng_object_new (self->server->engine, NULL, LIPHY_CONTROL_MODE_RIGID,
		liser_server_get_unique_object (self->server));
	if (object == NULL)
	{
		liscr_script_set_gc (self->server->script, 1);
		lisys_free (pass);
		return 0;
	}
	flags = lieng_object_get_flags (object);
	flags &= ~LIENG_OBJECT_FLAG_SAVE;
	lieng_object_set_flags (object, flags);

	/* Create client. */
	client = liser_client_new (self->server, object, message->NEW_USER.id);
	if (client == NULL)
	{
		grapple_server_disconnect_client (self->socket, message->NEW_USER.id);
		liscr_script_set_gc (self->server->script, 1);
		lisys_free (pass);
		return 0;
	}
	if (!lialg_u32dic_insert (self->clients, message->NEW_USER.id, client))
	{
		liser_client_free (client);
		liscr_script_set_gc (self->server->script, 1);
		lisys_free (pass);
		return 0;
	}

	/* Send resource list. */
	liser_client_send (client, self->resources, GRAPPLE_RELIABLE);

	/* Assign client to the object. */
	if (!liser_object_set_client (object, client))
	{
		liser_client_free (client);
		liscr_script_set_gc (self->server->script, 1);
		lisys_free (pass);
		return 0;
	}

	/* Invoke callbacks. */
	lical_callbacks_call (self->server->callbacks, self->server->engine, "client-login", lical_marshal_DATA_PTR_PTR_PTR, object, message->NEW_USER.name, pass);
	lisys_free (pass);
	liscr_script_set_gc (self->server->script, 1);

	return 1;
}

static int
private_disconnect (LISerNetwork*    self,
                    grapple_message* message)
{
	LISerClient* client;

	/* Get the client. */
	client = liser_network_find_client (self, message->USER_DISCONNECTED.id);
	if (client == NULL)
		return 0;
	assert (client->object != NULL);
	liser_object_disconnect (client->object);

	return 1;
}

static int
private_message (LISerNetwork*    self,
                 grapple_message* message)
{
	LIArcReader* reader;
	LISerClient* client;

	/* Get the client. */
	client = liser_network_find_client (self, message->USER_MSG.id);
	if (client == NULL)
		return 0;

	/* Create packet reader. */
	if (message->USER_MSG.length < 1)
		return 0;
	reader = liarc_reader_new (message->USER_MSG.data, message->USER_MSG.length);
	if (reader == NULL)
		return 0;
	reader->pos = 1;

	/* Invoke callbacks. */
	lical_callbacks_call (self->server->callbacks, self->server->engine, "client-packet", lical_marshal_DATA_PTR_PTR, client, reader);
	liarc_reader_free (reader);

	return 1;
}

static int
private_rename (LISerNetwork*    self,
                grapple_message* message)
{
	LISerClient* client;

	/* Get the client. */
	client = liser_network_find_client (self, message->USER_NAME.id);
	if (client == NULL)
		return 0;

	/* Not allowed. */
	return 0;
}

static int
private_update (LISerNetwork* self,
                float         secs)
{
	liser_network_update (self, secs);

	return 1;
}

/** @} */
/** @} */

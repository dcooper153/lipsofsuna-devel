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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtNetwork Network
 * @{
 */

#include <lipsofsuna/network.h>
#include "ext-module.h"
#include "ext-client.h"

#define CLIENT_REFRESH_RADIUS 10.0f

static int
private_init (LIExtModule* self);

static int
private_accept (grapple_user user,
                LIExtModule* self);

static int
private_login (const char*  login,
               const char*  password,
               LIExtModule* self);

static int
private_connect (LIExtModule*     self,
                 grapple_message* message);

static int
private_disconnect (LIExtModule*     self,
                    grapple_message* message);

static int
private_message_client (LIExtModule*     self,
                        grapple_message* message);

static int
private_message_server (LIExtModule*     self,
                        grapple_message* message);

static int
private_refused (LIExtModule*     self,
                 grapple_message* message);

static int
private_rename (LIExtModule*     self,
                grapple_message* message);

static int
private_update (LIExtModule* self,
                float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_info =
{
	LIMAI_EXTENSION_VERSION, "Network",
	liext_module_new,
	liext_module_free
};

LIExtModule*
liext_module_new (LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Connect callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "tick", 0, private_update, self, self->calls))
	{
		lisys_free (self);
		return NULL;
	}

	/* Initialize self. */
	if (!private_init (self))
	{
		liext_module_free (self);
		return NULL;
	}

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	LIAlgU32dicIter iter0;
	LIAlgStrdicIter iter1;

	if (self->clients != NULL)
	{
		LIALG_U32DIC_FOREACH (iter0, self->clients)
			liext_client_free (iter0.value);
		lialg_u32dic_free (self->clients);
	}
	if (self->objects != NULL)
		lialg_u32dic_free (self->objects);
	if (self->passwords != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->passwords)
			lisys_free (iter1.value);
		lialg_strdic_free (self->passwords);
	}
	if (self->resources != NULL)
		liarc_writer_free (self->resources);
	if (self->client_socket != 0)
		grapple_client_destroy (self->client_socket);
	if (self->server_socket != 0)
		grapple_server_destroy (self->server_socket);
	pthread_mutex_destroy (&self->mutex);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

int
liext_module_host (LIExtModule* self,
                   int          udp,
                   int          port)
{
	grapple_error error;

	/* Close old socket. */
	if (self->client_socket || self->server_socket)
		liext_module_shutdown (self);

	/* Create new socket. */
	self->server_socket = grapple_server_init ("Lips of Suna", LINET_PROTOCOL_VERSION);
	if (!self->server_socket)
	{
		lisys_error_set (EINVAL, "host: creating host socket failed");
		return 0;
	}
	grapple_server_notified_set (self->server_socket, GRAPPLE_NOTIFY_STATE_OFF);
	grapple_server_port_set (self->server_socket, port);
	grapple_server_protocol_set (self->server_socket, udp? GRAPPLE_PROTOCOL_UDP : GRAPPLE_PROTOCOL_TCP);
	grapple_server_session_set (self->server_socket, "");
	grapple_server_namepolicy_set (self->server_socket, GRAPPLE_NAMEPOLICY_REQUIRED);
	grapple_server_connectionhandler_set (self->server_socket, (grapple_connection_callback) private_accept, self);
	grapple_server_passwordhandler_set (self->server_socket, (grapple_password_callback) private_login, self);

	/* Wait for connections. */
	if (grapple_server_start (self->server_socket) != GRAPPLE_OK)
	{
		error = grapple_server_error_get (self->server_socket);
		lisys_error_set (EINVAL, "host: %s", grapple_error_text (error));
		grapple_server_destroy (self->server_socket);
		self->server_socket = 0;
		return 0;
	}

	return 1;
}

int
liext_module_join (LIExtModule* self,
                   int          udp,
                   int          port,
                   const char*  addr,
                   const char*  name,
                   const char*  pass)
{
	grapple_error error;

	/* Close old socket. */
	if (self->client_socket || self->server_socket)
		liext_module_shutdown (self);

	/* Create new socket. */
	self->client_socket = grapple_client_init ("Lips of Suna", LINET_PROTOCOL_VERSION);
	if (!self->client_socket)
		return 0;
	grapple_client_address_set (self->client_socket, addr);
	grapple_client_port_set (self->client_socket, port);
	if (udp)
		grapple_client_protocol_set (self->client_socket, GRAPPLE_PROTOCOL_UDP);
	else
		grapple_client_protocol_set (self->client_socket, GRAPPLE_PROTOCOL_TCP);
	grapple_client_name_set (self->client_socket, name);
	grapple_client_password_set (self->client_socket, pass);

	/* Connect to the host. */
	if (grapple_client_start (self->client_socket, 0) != GRAPPLE_OK)
	{
		error = grapple_client_error_get (self->client_socket);
		lisys_error_set (EINVAL, "connect: %s", grapple_error_text (error));
		grapple_client_destroy (self->client_socket);
		self->client_socket = 0;
		return 0;
	}

	return 1;
}

void
liext_module_update (LIExtModule* self,
                     float        secs)
{
	int ret;
	LIAlgU32dicIter iter;
	LIExtClient* client;
	grapple_message* message;

	/* Handle client socket. */
	if (self->client_socket)
	{
		while (grapple_client_messages_waiting (self->client_socket))
		{
			message = grapple_client_message_pull (self->client_socket);
			switch (message->type)
			{
				case GRAPPLE_MSG_NEW_USER_ME:
				case GRAPPLE_MSG_USER_NAME:
				case GRAPPLE_MSG_SESSION_NAME:
					ret = 1;
					break;
				case GRAPPLE_MSG_USER_MSG:
					ret = private_message_client (self, message);
					break;
				case GRAPPLE_MSG_CONNECTION_REFUSED:
					ret = private_refused (self, message);
					break;
				case GRAPPLE_MSG_SERVER_DISCONNECTED:
					lisys_error_set (EINVAL, "disconnected");
					ret = 0;
					break;
				default:
					ret = 1;
					break;
			}
			grapple_message_dispose (message);
			if (!ret)
			{
				lisys_error_report ();
				liext_module_shutdown (self);
			}
		}
	}

	/* Handle server socket. */
	if (self->server_socket)
	{
		if (grapple_server_messages_waiting (self->server_socket))
		{
			message = grapple_server_message_pull (self->server_socket);
			switch (message->type)
			{
				case GRAPPLE_MSG_NEW_USER:
					private_connect (self, message);
					break;
				case GRAPPLE_MSG_USER_NAME:
					private_rename (self, message);
					break;
				case GRAPPLE_MSG_USER_MSG:
					private_message_server (self, message);
					break;
				case GRAPPLE_MSG_USER_DISCONNECTED:
					private_disconnect (self, message);
					break;
				default:
					break;
			}
			grapple_message_dispose (message);
		}

		/* Prevent sectors from unloading. */
		LIALG_U32DIC_FOREACH (iter, self->clients)
		{
			client = iter.value;
			if (client->object != NULL)
				lieng_object_refresh (client->object, CLIENT_REFRESH_RADIUS);
		}
	}
}

/**
 * \brief Find a client by network user.
 *
 * \param self Network.
 * \param user Network user.
 * \return Client or NULL.
 */
LIExtClient*
liext_module_find_client (LIExtModule* self,
                          grapple_user user)
{
	return lialg_u32dic_find (self->clients, user);
}

/**
 * \brief Find a client by object ID.
 *
 * \param self Network.
 * \param id Object ID.
 * \return Client or NULL.
 */
LIExtClient*
liext_module_find_client_by_object (LIExtModule* self,
                                    uint32_t     id)
{
	return lialg_u32dic_find (self->objects, id);
}

int
liext_module_send (LIExtModule* self,
                   LIEngObject* object,
                   LIArcWriter* writer,
                   int          flags)
{
	LIExtClient* client;

	if (self->client_socket)
	{
		assert (object == NULL);
		grapple_client_send (self->client_socket, GRAPPLE_SERVER, flags,
			liarc_writer_get_buffer (writer),
			liarc_writer_get_length (writer));
		return 1;
	}
	if (self->server_socket)
	{
		assert (object != NULL);
		client = liext_module_find_client_by_object (self, object->id);
		if (client == NULL)
			return 0;
		liext_client_send (client, writer, flags);
		return 1;
	}

	return 0;
}

void
liext_module_shutdown (LIExtModule* self)
{
	LIAlgU32dicIter iter0;
	LIAlgStrdicIter iter1;

	/* Disconnect and free clients. */
	LIALG_U32DIC_FOREACH (iter0, self->clients)
		liext_client_free (iter0.value);
	lialg_u32dic_clear (self->clients);
	assert (self->objects->size == 0);

	/* Clear pending authorization requests. */
	LIALG_STRDIC_FOREACH (iter1, self->passwords)
		lisys_free (iter1.value);
	lialg_strdic_clear (self->passwords);

	/* Destroy socket. */
	if (self->client_socket)
	{
		grapple_client_destroy (self->client_socket);
		self->client_socket = 0;
	}
	if (self->server_socket)
	{
		grapple_server_destroy (self->server_socket);
		self->server_socket = 0;
	}
}

int
liext_module_get_connected (LIExtModule* self)
{
	return self->client_socket != 0 || self->server_socket != 0;
}

/*****************************************************************************/

static int
private_init (LIExtModule* self)
{
	int i;
	LIEngAnimation* anim;
	LIEngModel* model;
	LIEngSample* sample;

	pthread_mutex_init (&self->mutex, NULL);

	/* Allocate client list. */
	self->clients = lialg_u32dic_new ();
	if (self->clients == NULL)
		return 0;
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
		return 0;
	self->passwords = lialg_strdic_new ();
	if (self->passwords == NULL)
		return 0;

	/* Build resource packet. */
	/* TODO: Should use compression. */
	self->resources = liarc_writer_new_packet (LINET_SERVER_PACKET_RESOURCES);
	if (self->resources == NULL)
		return 0;
	liarc_writer_append_uint32 (self->resources, self->program->engine->resources->animations.count);
	liarc_writer_append_uint32 (self->resources, self->program->engine->resources->models.count);
	liarc_writer_append_uint32 (self->resources, self->program->engine->resources->samples.count);
	for (i = 0 ; i < self->program->engine->resources->animations.count ; i++)
	{
		anim = lieng_resources_find_animation_by_code (self->program->engine->resources, i);
		assert (anim != NULL);
		liarc_writer_append_string (self->resources, anim->name);
		liarc_writer_append_nul (self->resources);
	}
	for (i = 0 ; i < self->program->engine->resources->models.count ; i++)
	{
		model = lieng_resources_find_model_by_code (self->program->engine->resources, i);
		assert (model != NULL);
		liarc_writer_append_string (self->resources, model->name);
		liarc_writer_append_nul (self->resources);
	}
	for (i = 0 ; i < self->program->engine->resources->samples.count ; i++)
	{
		sample = lieng_resources_find_sample_by_code (self->program->engine->resources, i);
		assert (sample != NULL);
		liarc_writer_append_string (self->resources, sample->name);
		liarc_writer_append_nul (self->resources);
	}

	/* Register classes. */
	liscr_script_create_class (self->program->script, "Network", liext_script_network, self);

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
private_accept (grapple_user user,
                LIExtModule* self)
{
	int ret = 1;
	char* address;

	/* Get client address. */
	address = grapple_server_client_address_get (self->server_socket, user);
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
               LIExtModule* self)
{
	int ret = 1;

	/* Sanity checks. */
	if (login == NULL || password == NULL || !strlen (login) || !strlen (password))
		return 0;

	/* TODO: Check for account. */

	return ret;
}

static int
private_connect (LIExtModule*     self,
                 grapple_message* message)
{
	int flags;
	char* pass;
	LIEngObject* object;
	LIExtClient* client;

	/* Get temporarily stored password. */
	pthread_mutex_lock (&self->mutex);
	pass = lialg_strdic_find (self->passwords, message->NEW_USER.name);
	if (pass != NULL)
		lialg_strdic_remove (self->passwords, message->NEW_USER.name);
	pthread_mutex_unlock (&self->mutex);

	/* Create object. */
	liscr_script_set_gc (self->program->script, 0);
	object = lieng_object_new (self->program->engine, NULL, LIPHY_CONTROL_MODE_RIGID, 0);
	if (object == NULL)
	{
		liscr_script_set_gc (self->program->script, 1);
		lisys_free (pass);
		return 0;
	}
	flags = lieng_object_get_flags (object);
	flags &= ~LIENG_OBJECT_FLAG_SAVE;
	lieng_object_set_flags (object, flags);

	/* Create client. */
	client = liext_client_new (self, object, message->NEW_USER.id);
	if (client == NULL)
	{
		grapple_server_disconnect_client (self->server_socket, message->NEW_USER.id);
		liscr_script_set_gc (self->program->script, 1);
		lisys_free (pass);
		return 0;
	}
	if (!lialg_u32dic_insert (self->clients, message->NEW_USER.id, client))
	{
		liext_client_free (client);
		liscr_script_set_gc (self->program->script, 1);
		lisys_free (pass);
		return 0;
	}

	/* Send resource list. */
	liext_client_send (client, self->resources, GRAPPLE_RELIABLE);

	/* Assign client to the object. */
	if (!liext_client_set_object (client, object))
	{
		liext_client_free (client);
		liscr_script_set_gc (self->program->script, 1);
		lisys_free (pass);
		return 0;
	}

	/* Invoke callbacks. */
	lical_callbacks_call (self->program->callbacks, self->program->engine, "client-login", lical_marshal_DATA_PTR_PTR_PTR, object, message->NEW_USER.name, pass);
	lisys_free (pass);
	liscr_script_set_gc (self->program->script, 1);

	return 1;
}

static int
private_disconnect (LIExtModule*     self,
                    grapple_message* message)
{
	LIExtClient* client;

	/* Get the client. */
	client = liext_module_find_client (self, message->USER_DISCONNECTED.id);
	if (client == NULL)
		return 0;
	assert (client->object != NULL);
	liext_client_free (client);

	return 1;
}

static int
private_message_client (LIExtModule*     self,
                        grapple_message* message)
{
	int len;
	const uint8_t* data;
	LIArcReader* reader;

	/* Check for valid length. */
	len = message->USER_MSG.length;
	data = message->USER_MSG.data;
	if (len < 1)
	{
		lisys_error_set (EINVAL, "invalid packet size");
		return 0;
	}

	/* Create packet reader. */
	reader = liarc_reader_new ((char*) data, len);
	if (reader == NULL)
		return 0;
	reader->pos = 1;

	/* Handle resource list. */
	/* FIXME: This shouldn't be hardcoded either. */
	if (data[0] == LINET_SERVER_PACKET_RESOURCES)
		lieng_engine_load_resources (self->program->engine, reader);

	/* Invoke callbacks. */
	/* FIXME: Would be better if this was consistent with the server equivalent. */
	lical_callbacks_call (self->program->callbacks, self->program->engine, "packet", lical_marshal_DATA_INT_PTR, (int) data[0], reader);
	liarc_reader_free (reader);

	return 1;
}

static int
private_message_server (LIExtModule*     self,
                        grapple_message* message)
{
	LIArcReader* reader;
	LIExtClient* client;

	/* Get the client. */
	client = liext_module_find_client (self, message->USER_MSG.id);
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
	lical_callbacks_call (self->program->callbacks, self->program->engine, "client-packet", lical_marshal_DATA_PTR_PTR, client->object, reader);
	liarc_reader_free (reader);

	return 1;
}

static int
private_refused (LIExtModule*     self,
                 grapple_message* message)
{
	switch (message->CONNECTION_REFUSED.reason)
	{
		case GRAPPLE_NOCONN_VERSION_MISMATCH:
			lisys_error_set (LISYS_ERROR_VERSION, "incompatible server version");
			break;
		case GRAPPLE_NOCONN_SERVER_FULL:
			lisys_error_set (LISYS_ERROR_UNKNOWN, "server is full");
			break;
		case GRAPPLE_NOCONN_SERVER_CLOSED:
			lisys_error_set (LISYS_ERROR_UNKNOWN, "server is closed");
			break;
		case GRAPPLE_NOCONN_PASSWORD_MISMATCH:
			lisys_error_set (LISYS_ERROR_UNKNOWN, "invalid password");
			break;
		case GRAPPLE_NOCONN_NAME_NOT_UNIQUE:
			lisys_error_set (LISYS_ERROR_UNKNOWN, "account already in use");
			break;
		default:
			lisys_error_set (LISYS_ERROR_UNKNOWN, "unknown error %d", message->CONNECTION_REFUSED.reason);
			break;
	}
	return 0;
}

static int
private_rename (LIExtModule*     self,
                grapple_message* message)
{
	LIExtClient* client;

	/* Get the client. */
	client = liext_module_find_client (self, message->USER_NAME.id);
	if (client == NULL)
		return 0;

	/* Not allowed. */
	return 0;
}

static int
private_update (LIExtModule* self,
                float        secs)
{
	liext_module_update (self, secs);

	return 1;
}

/** @} */
/** @} */

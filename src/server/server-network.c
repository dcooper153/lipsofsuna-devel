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
 * \addtogroup lisrvNetwork Network
 * @{
 */

#include <network/lips-network.h>
#include "server-network.h"

static int
private_init (lisrvNetwork* self,
              int           udp,
              int           port);

static int
private_accept (grapple_user  user,
                lisrvNetwork* self);

static int
private_login (const char*   login,
               const char*   password,
               lisrvNetwork* self);

static int
private_connect (lisrvNetwork*    self,
                 grapple_message* message);

static int
private_disconnect (lisrvNetwork*    self,
                    grapple_message* message);

static int
private_message (lisrvNetwork*    self,
                 grapple_message* message);

static int
private_rename (lisrvNetwork*    self,
                grapple_message* message);

/*****************************************************************************/

lisrvNetwork*
lisrv_network_new (lisrvServer* server,
                   int          udp,
                   int          port)
{
	lisrvNetwork* self;

	self = calloc (1, sizeof (lisrvNetwork));
	if (self == NULL)
		return NULL;
	self->server = server;

	pthread_mutex_init (&self->mutex, NULL);
	if (!private_init (self, udp, port))
	{
		lisrv_network_free (self);
		return NULL;
	}

	return self;
}

void
lisrv_network_free (lisrvNetwork* self)
{
	lialgU32dicIter iter0;
	lialgStrdicIter iter1;

	if (self->clients != NULL)
	{
		LI_FOREACH_U32DIC (iter0, self->clients)
			lisrv_client_free (iter0.value);
		lialg_u32dic_free (self->clients);
	}
	if (self->passwords != NULL)
	{
		LI_FOREACH_STRDIC (iter1, self->passwords)
			free (iter1.value);
		lialg_strdic_free (self->passwords);
	}
	if (self->server != 0)
		grapple_server_destroy (self->socket);
	pthread_mutex_destroy (&self->mutex);
	free (self);
}

void
lisrv_network_update (lisrvNetwork* self,
                      float         secs)
{
	grapple_message* message;

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
}

/**
 * \brief Find a client by network user.
 *
 * \param self Network.
 * \param user Network user.
 * \return Client or NULL.
 */
lisrvClient*
lisrv_network_find_client (lisrvNetwork* self,
                           grapple_user  user)
{
	return lialg_u32dic_find (self->clients, user);
}

/*****************************************************************************/

static int
private_init (lisrvNetwork* self,
              int           udp,
              int           port)
{
	/* Allocate client list. */
	self->clients = lialg_u32dic_new ();
	if (self->clients == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}
	self->passwords = lialg_strdic_new ();
	if (self->passwords == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
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
	if (!grapple_server_start (self->socket))
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "cannot start grapple server");
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
                lisrvNetwork* self)
{
	int ret;
	char* address;

	/* Get client address. */
	address = grapple_server_client_address_get (self->socket, user);
	if (address == NULL)
		return 0;

	/* Check if banned. */
	pthread_mutex_lock (&self->server->mutexes.bans);
	ret = lisrv_server_get_banned (self->server, address);
	pthread_mutex_unlock (&self->server->mutexes.bans);
	free (address);
	return !ret;
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
               lisrvNetwork* self)
{
	int ret;
	char* tmp;
	char* path;
	licfgAccount* account;

	/* Sanity checks. */
	if (login == NULL || password == NULL || !strlen (login) || !strlen (password))
		return 0;

	/* Contruct the account path. */
	path = lisys_path_concat (self->server->paths->server_state, "accounts", login, NULL);
	if (path == NULL)
		return 0;

	/* Parse the account file. */
#warning FIXME: Accounts should be done in a module.
	account = licfg_account_new (path);
	free (path);
	if (account == NULL)
	{
		if (1/*self->config.server->enable_create_account*/)
		{
			if (lisys_error_peek () == EIO)
			{
				/* Store the password. */
				/* It will be retrieved when creating a new account and
				   used as the password of the account. */
				tmp = strdup (password);
				if (tmp == NULL)
					return 0;
				pthread_mutex_lock (&self->mutex);
				ret = (lialg_strdic_insert (self->passwords, login, tmp) != NULL);
				pthread_mutex_unlock (&self->mutex);
				if (!ret)
				{
					free (tmp);
					return 0;
				}
				return 1;
			}
		}
		return 0;
	}

	/* Check for valid password. */
	ret = !strcmp (account->password, password);
	licfg_account_free (account);
	return ret;
}

static int
private_connect (lisrvNetwork*    self,
                 grapple_message* message)
{
	int flags;
	char* pass;
	liengObject* object;
	lisrvClient* client;

	/* Get temporarily stored password. */
	pthread_mutex_lock (&self->mutex);
	pass = lialg_strdic_find (self->passwords, message->NEW_USER.name);
	if (pass != NULL)
		lialg_strdic_remove (self->passwords, message->NEW_USER.name);
	pthread_mutex_unlock (&self->mutex);

	/* Create object. */
	object = lieng_object_new (self->server->engine, NULL,
		LIPHY_SHAPE_MODE_CONVEX, LIPHY_CONTROL_MODE_RIGID, 0, NULL);
	if (object == NULL)
	{
		free (pass);
		return 0;
	}
	flags = lieng_object_get_flags (object);
	flags &= ~LIENG_OBJECT_FLAG_SAVE;
	lieng_object_set_flags (object, flags);

	/* Create client. */
	client = lisrv_client_new (self->server, object, message->NEW_USER.id);
	if (client == NULL)
	{
		grapple_server_disconnect_client (self->socket, message->NEW_USER.id);
		lisrv_object_free (object);
		free (pass);
		return 0;
	}
	if (!lialg_u32dic_insert (self->clients, message->NEW_USER.id, client))
	{
		lisrv_object_free (object);
		lisrv_client_free (client);
		free (pass);
		return 0;
	}

	/* Send resource list. */
	lisrv_client_send (client, self->server->helper.resources, GRAPPLE_RELIABLE);

	/* Assign client to the object. */
	if (!lisrv_object_set_client (object, client))
	{
		lisrv_client_free (client);
		lisrv_object_free (object);
		free (pass);
		return 0;
	}

	/* Invoke callbacks. */
	lieng_engine_call (self->server->engine, LISRV_CALLBACK_CLIENT_LOGIN, object, message->NEW_USER.name, pass);
	free (pass);

	return 1;
}

static int
private_disconnect (lisrvNetwork*    self,
                    grapple_message* message)
{
	lisrvClient* client;

	/* Get the client. */
	client = lisrv_network_find_client (self, message->USER_DISCONNECTED.id);
	if (client == NULL)
		return 0;
	assert (client->object != NULL);
	lisrv_object_disconnect (client->object);

	return 1;
}

static int
private_message (lisrvNetwork*    self,
                 grapple_message* message)
{
	liReader* reader;
	lisrvClient* client;

	/* Get the client. */
	client = lisrv_network_find_client (self, message->USER_MSG.id);
	if (client == NULL)
		return 0;

	/* Create packet reader. */
	if (message->USER_MSG.length < 1)
		return 0;
	reader = li_reader_new (message->USER_MSG.data, message->USER_MSG.length);
	if (reader == NULL)
		return 0;
	reader->pos = 1;

	/* Invoke callbacks. */
	lieng_engine_call (self->server->engine, LISRV_CALLBACK_CLIENT_PACKET, client, reader);
	li_reader_free (reader);

	return 1;
}

static int
private_rename (lisrvNetwork*    self,
                grapple_message* message)
{
	lisrvClient* client;

	/* Get the client. */
	client = lisrv_network_find_client (self, message->USER_NAME.id);
	if (client == NULL)
		return 0;

	/* Not allowed. */
	return 0;
}

/** @} */
/** @} */

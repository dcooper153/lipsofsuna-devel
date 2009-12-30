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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliNetwork Network
 * @{
 */

#include "client.h"
#include "client-network.h"

#define LICLI_NETWORK_INPUT_LATENCY 10
#define LICLI_NETWORK_MOVEMENT_ACCUM 10.0f
#define LICLI_NETWORK_ROTATION_ACCUM 200.0f

static int
private_message (licliNetwork*    self,
                 grapple_message* message);

static int
private_refused (licliNetwork*    self,
                 grapple_message* message);

/*****************************************************************************/

/**
 * \brief Creates a new client network interface.
 *
 * \param module Module.
 * \param name Login name.
 * \param pass Login password.
 * \return Network interface.
 */
licliNetwork*
licli_network_new (licliModule* module,
                   const char*  name,
                   const char*  pass)
{
	grapple_error error;
	licliNetwork* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (licliNetwork));
	if (self == NULL)
		return NULL;
	self->module = module;

	/* Read configuration. */
	self->host = licfg_host_new (module->path);
	if (self->host == NULL)
		goto error;

	/* Connect to the host. */
	self->client = grapple_client_init ("Lips of Suna", LINET_PROTOCOL_VERSION);
	if (self->client == 0)
		goto error;
	grapple_client_address_set (self->client, self->host->host);
	grapple_client_port_set (self->client, self->host->port);
	if (self->host->udp)
		grapple_client_protocol_set (self->client, GRAPPLE_PROTOCOL_UDP);
	else
		grapple_client_protocol_set (self->client, GRAPPLE_PROTOCOL_TCP);
	grapple_client_name_set (self->client, name);
	grapple_client_password_set (self->client, pass);
	if (grapple_client_start (self->client, 0) != GRAPPLE_OK)
	{
		error = grapple_client_error_get (self->client);
		lisys_error_set (EINVAL, "connect: %s", grapple_error_text (error));
		goto error;
	}

	return self;

error:
	lisys_error_append ("cannot connect to server");
	licli_network_free (self);
	return NULL;
}

/**
 * \brief Frees the network interface.
 *
 * \param self Network interface.
 */
void
licli_network_free (licliNetwork* self)
{
	if (self->host != NULL)
		licfg_host_free (self->host);
	if (self->client)
		grapple_client_destroy (self->client);
	lisys_free (self);
}

/**
 * \brief Sets the tilting rate of the player.
 *
 * \param self Network interface.
 * \param value Tilting rate.
 * \param keep Nonzero if should keep tilting at this rate.
 */
void
licli_network_tilt (licliNetwork* self,
                    float         value,
                    int           keep)
{
	limatQuaternion quat;
	limatVector axis;

	if (!keep)
	{
		axis = limat_vector_init (1.0f, 0.0f, 0.0f);
		quat = limat_quaternion_rotation (-value, axis);
		quat = limat_quaternion_multiply (self->curr.direction, quat);
		quat = limat_quaternion_normalize (quat);
		self->curr.direction = quat;
	}
	else
		self->curr.controls.tilt = value;
}

/**
 * \brief Sets the turning rate of the player.
 *
 * \param self Network interface.
 * \param value Turning rate.
 * \param keep Nonzero if should keep turning at this rate.
 */
void
licli_network_turn (licliNetwork* self,
                    float         value,
                    int           keep)
{
	limatQuaternion quat;
	limatVector axis;

	if (!keep)
	{
		quat = limat_quaternion_conjugate (self->curr.direction);
		axis = limat_vector_init (0.0f, 1.0f, 0.0f);
		axis = limat_quaternion_transform (quat, axis);
		quat = limat_quaternion_rotation (-value, axis);
		quat = limat_quaternion_multiply (self->curr.direction, quat);
		quat = limat_quaternion_normalize (quat);
		self->curr.direction = quat;
	}
	else
		self->curr.controls.turn = value;
}

/**
 * \brief Updates the network interface state.
 *
 * \param self Network interface.
 * \param secs Update tick length in seconds.
 * \return Nonzero on success.
 */
int
licli_network_update (licliNetwork* self,
                      float         secs)
{
	int ret;
	float diff;
	liarcWriter* writer;
	limatQuaternion quat;
	grapple_message* message;

	if (!self->client)
		return 0;

	/* Read packets. */
	while (grapple_client_messages_waiting (self->client))
	{
		message = grapple_client_message_pull (self->client);
		switch (message->type)
		{
			case GRAPPLE_MSG_NEW_USER_ME:
			case GRAPPLE_MSG_USER_NAME:
			case GRAPPLE_MSG_SESSION_NAME:
				ret = 1;
				break;
			case GRAPPLE_MSG_USER_MSG:
				ret = private_message (self, message);
				break;
			case GRAPPLE_MSG_CONNECTION_REFUSED:
				ret = private_refused (self, message);
				break;
			case GRAPPLE_MSG_SERVER_DISCONNECTED:
				ret = 0;
				break;
			default:
				printf ("WARNING: Unkown message %d\n", message->type);
				ret = 1;
				break;
		}
		grapple_message_dispose (message);
		if (!ret)
		{
			grapple_client_destroy (self->client);
			self->client = 0;
			return 0;
		}
	}

	/* Update controls. */
	licli_network_turn (self, -self->curr.controls.turn * secs * LI_CLIENT_ROTATION_SPEED, 0);
	licli_network_tilt (self, -self->curr.controls.tilt * secs * LI_CLIENT_ROTATION_SPEED, 0);

	/* Update cumulative orientation errors. */
	diff = LI_ABS (self->curr.controls.move - self->prev.controls.move);
	self->delta.movement += diff * secs * LICLI_NETWORK_MOVEMENT_ACCUM;
	quat = limat_quaternion_subtract (self->curr.direction, self->prev.direction);
	diff = limat_quaternion_get_length (quat);
	self->delta.rotation += diff * secs * LICLI_NETWORK_ROTATION_ACCUM;

	/* Synchronize controls. */
	if (licli_network_get_dirty (self))
	{
		self->delta.tick = self->module->client->video.SDL_GetTicks ();
		self->delta.movement = 0.0f;
		self->delta.rotation = 0.0f;
		self->prev.controls = self->curr.controls;
		self->prev.direction = self->curr.direction;
		writer = liarc_writer_new_packet (LINET_CLIENT_PACKET_MOVE);
		if (writer != NULL)
		{
			/* FIXME: No analog support. */
			uint32_t flags = 0;
			if (self->curr.controls.move > 0.1f) flags |= LI_CONTROL_MOVE_FRONT;
			if (self->curr.controls.move < -0.1f) flags |= LI_CONTROL_MOVE_BACK;
			liarc_writer_append_uint8 (writer, flags);
			liarc_writer_append_int8 (writer, (int8_t)(127 * self->curr.direction.x));
			liarc_writer_append_int8 (writer, (int8_t)(127 * self->curr.direction.y));
			liarc_writer_append_int8 (writer, (int8_t)(127 * self->curr.direction.z));
			liarc_writer_append_int8 (writer, (int8_t)(127 * self->curr.direction.w));
			licli_module_send (self->module, writer, 0);
			liarc_writer_free (writer);
		}
	}

	return 1;
}

/**
 * \brief Checks if the client is connected.
 *
 * \param self Network interface.
 * \return Nonzero if connected.
 */
int
licli_network_get_connected (const licliNetwork* self)
{
	return self->client != 0;
}

/**
 * \brief Checks if the control state needs to be synchronized.
 *
 * \param self Network interface.
 * \return Nonzero if synchronization is needed.
 */
int
licli_network_get_dirty (const licliNetwork* self)
{
	Uint32 ticks = self->module->client->video.SDL_GetTicks ();

	if (ticks - self->delta.tick < LICLI_NETWORK_INPUT_LATENCY)
		return 0;
	if (self->delta.movement > 1.0f ||
	    self->delta.rotation > 1.0f)
		return 1;
	return 0;
}

/**
 * \brief Gets the current rotation of the player.
 *
 * \param self Network interface.
 * \param value Return location for the quaternion.
 */
void
licli_network_get_rotation (const licliNetwork* self,
                            limatQuaternion*    value)
{
	*value = self->curr.direction;
}

/**
 * \brief Sets the current rotation of the player.
 *
 * \param self Network interface.
 * \param value Rotation value.
 */
void
licli_network_set_rotation (licliNetwork*          self,
                            const limatQuaternion* value)
{
	self->curr.direction = *value;
}

/*****************************************************************************/

static int
private_message (licliNetwork*    self,
                 grapple_message* message)
{
	int len;
	const uint8_t* data;
	liarcReader* reader;

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

	/* Invoke callbacks. */
	lical_callbacks_call (self->module->callbacks, self->module->engine, "packet", lical_marshal_DATA_INT_PTR, (int) data[0], reader);
	liarc_reader_free (reader);

	return 1;
}

static int
private_refused (licliNetwork*    self,
                 grapple_message* message)
{
	switch (message->CONNECTION_REFUSED.reason)
	{
		case GRAPPLE_NOCONN_VERSION_MISMATCH:
			lisys_error_set (LI_ERROR_VERSION, "incompatible server version");
			break;
		case GRAPPLE_NOCONN_SERVER_FULL:
			lisys_error_set (LI_ERROR_UNKNOWN, "server is full");
			break;
		case GRAPPLE_NOCONN_SERVER_CLOSED:
			lisys_error_set (LI_ERROR_UNKNOWN, "server is closed");
			break;
		case GRAPPLE_NOCONN_PASSWORD_MISMATCH:
			lisys_error_set (LI_ERROR_UNKNOWN, "invalid password");
			break;
		case GRAPPLE_NOCONN_NAME_NOT_UNIQUE:
			lisys_error_set (LI_ERROR_UNKNOWN, "account already in use");
			break;
		default:
			lisys_error_set (LI_ERROR_UNKNOWN, "unknown error %d", message->CONNECTION_REFUSED.reason);
			break;
	}
	return 0;
}

/** @} */
/** @} */

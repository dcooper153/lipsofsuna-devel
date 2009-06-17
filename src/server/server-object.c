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
 * \addtogroup lisrvObject Object
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <network/lips-network.h>
#include <physics/lips-physics.h>
#include "server.h"
#include "server-callbacks.h"
#include "server-client.h"
#include "server-object.h"
#include "server-observer.h"
#include "server-script.h"

#define LISRV_OBJECT_VERSION 0

static int
private_delete_animations (liengObject* self);

static int
private_delete_variables (liengObject* self);

static int
private_read_animations (liengObject* self);

static int
private_read_variables (liengObject* self);

static int
private_write_animations (liengObject* self);

static int
private_write_variables (liengObject* self);

/*****************************************************************************/

/**
 * \brief Creates a new server object.
 *
 * \param engine Engine.
 * \param model Model or NULL.
 * \param shape_mode Collision shape type.
 * \param control_mode Interaction type.
 * \param id Object number or zero.
 * \param ptr Data passed to engine.
 * \return New object or NULL.
 */
liengObject*
lisrv_object_new (liengEngine*     engine,
                  liengModel*      model,
                  liphyShapeMode   shape_mode,
                  liphyControlMode control_mode,
                  uint32_t         id,
                  void*            ptr)
{
	int ret;
	double rnd;
	const char* query;
	liengObject* self;
	lisrvObject* data;
	lisrvServer* server = lieng_engine_get_userdata (engine, LIENG_DATA_SERVER);
	sqlite3_stmt* statement;

	/* Choose unique object number. */
	while (!id)
	{
		/* Choose random number. */
		rnd = rand () / (double) RAND_MAX;
		id = engine->range.start + (uint32_t)(engine->range.size * rnd);
		if (!id)
			continue;

		/* Reject numbers of loaded objects. */
		if (lialg_u32dic_find (engine->objects, id))
		{
			id = 0;
			continue;
		}

		/* Reject numbers of database objects. */
		query = "SELECT id FROM objects WHERE id=?;";
		if (sqlite3_prepare_v2 (server->sql, query, -1, &statement, NULL) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (server->sql));
			return 0;
		}
		if (sqlite3_bind_int (statement, 1, id) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (server->sql));
			sqlite3_finalize (statement);
			return 0;
		}
		ret = sqlite3_step (statement);
		if (ret != SQLITE_DONE)
		{
			if (ret != SQLITE_ROW)
			{
				lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (server->sql));
				sqlite3_finalize (statement);
				return 0;
			}
			id = 0;
		}
		sqlite3_finalize (statement);
	}

	/* Allocate engine data. */
	self = lieng_default_calls.lieng_object_new (engine, model, shape_mode, control_mode, id, ptr);
	if (self == NULL)
		return NULL;

	/* Allocate server data. */
	data = calloc (1, sizeof (lisrvObject));
	if (data == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}
	data->server = server;
	data->animations = lialg_u32dic_new ();
	if (data->animations == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}

	/* Extend engine object. */
	lieng_object_set_userdata (self, LIENG_DATA_SERVER, data);
	liphy_object_set_userdata (self->physics, self);

	/* Allocate script data. */
	self->script = liscr_data_new (server->script, self, LICOM_SCRIPT_OBJECT);
	if (self->script == NULL)
		goto error;
	liscr_data_unref (self->script, NULL);

	return self;

error:
	lieng_object_free (self);
	return NULL;
}

/**
 * \brief Marks the object as deleted.
 *
 * \param self Object.
 */
void
lisrv_object_free (liengObject* self)
{
	lialgU32dicIter iter;
	lisrvObject* data = LISRV_OBJECT (self);

	/* Free client. */
	lisrv_object_disconnect (self);

	/* Unrealize before server data is freed. */
	lieng_object_set_realized (self, 0);

	/* Free server data. */
	if (data != NULL)
	{
		LI_FOREACH_U32DIC (iter, data->animations)
			free (iter.value);
		lialg_u32dic_free (data->animations);
		free (data->name);
		free (data);
	}

	/* Free engine data. */
	lieng_default_calls.lieng_object_free (self);
}

/**
 * \brief Plays back an animation.
 *
 * \param self Object.
 * \param name Animation name or NULL.
 * \param channel Channel to use, -1 for the largest free channel number.
 * \param priority Blending priority.
 * \param permanent Nonzero if permanent, zero if temporary.
 * \return Nonzero on success.
 */
int
lisrv_object_animate (liengObject* self,
                      const char*  name,
                      int          channel,
                      float        priority,
                      int          permanent)
{
	liengAnimation* animation;
	lisrvAniminfo tmp;
	lisrvAniminfo* info;
	lisrvObject* data = LISRV_OBJECT (self);

	/* Find animation channel. */
	if (channel == -1)
	{
		for (channel = 254 ; channel > 0 ; channel--)
		{
			if (!lialg_u32dic_find (data->animations, channel))
				break;
		}
	}

	/* Find animation number. */
	if (name != NULL)
		animation = lieng_engine_find_animation_by_name (self->engine, name);
	else
		animation = NULL;

	/* Check for duplicate. */
	info = lialg_u32dic_find (data->animations, channel);
	if (info != NULL)
	{
		if (info->animation == animation &&
		    info->channel == channel &&
		    info->permanent == permanent &&
		    info->priority == priority)
			return 1;
	}

	/* Clear channel. */
	if (info != NULL)
	{
		lialg_u32dic_remove (data->animations, channel);
		free (info);
	}
	if (name != NULL && animation == NULL)
		return 0;

	/* Register channel if permanent. */
	if (permanent)
	{
		info = calloc (1, sizeof (lisrvAniminfo));
		if (info == NULL)
			return 0;
		info->animation = animation;
		info->channel = channel;
		info->permanent = permanent;
		info->priority = priority;
		if (!lialg_u32dic_insert (data->animations, channel, info))
		{
			free (info);
			return 0;
		}
	}

	/* Invoke callbacks. */
	if (lieng_object_get_realized (self))
	{
		tmp.animation = animation;
		tmp.channel = channel;
		tmp.permanent = permanent;
		tmp.priority = priority;
		lieng_engine_call (self->engine, LISRV_CALLBACK_OBJECT_ANIMATION, self, &tmp);
	}

	return 1;
}

/**
 * \brief Disconnects the client controlling the object.
 *
 * \param self Object.
 */
void
lisrv_object_disconnect (liengObject* self)
{
	lisrvObject* data = LISRV_OBJECT (self);

	if (data->client != NULL)
	{
		lieng_engine_call (self->engine, LISRV_CALLBACK_CLIENT_LOGOUT, self);
		lisrv_client_free (data->client);
		data->client = NULL;
		lieng_object_ref (self, -1);
	}
}

/**
 * \brief Emit a sound or graphical effect.
 *
 * \param self Object.
 * \param value Effect name.
 * \param flags Effect flags.
 */
void
lisrv_object_effect (liengObject* self,
                     const char*  value,
                     int          flags)
{
	liengSample* sample;
	lisrvObject* data = LISRV_OBJECT (self);

	/* Find effect. */
	if (!lieng_object_get_realized (self))
		return;
	sample = lieng_resources_find_sample_by_name (data->server->engine->resources, value);
	if (sample == NULL)
		return;

	/* Invoke callbacks. */
	lieng_engine_call (self->engine, LISRV_CALLBACK_OBJECT_SAMPLE, self, sample, flags);
}

/**
 * \brief Called by the engine every time the object moves.
 *
 * \param self Object.
 */
int
lisrv_object_moved (liengObject* self)
{
	/* Call base. */
	if (!lieng_default_calls.lieng_object_moved (self))
		return 0;

	/* Invoke callbacks. */
	lieng_engine_call (self->engine, LISRV_CALLBACK_OBJECT_MOTION, self);

	return 1;
}

/**
 * \brief Purges the object from the object database.
 *
 * \param self Object.
 * \return Nonzero on success.
 */
int
lisrv_object_purge (liengObject* self)
{
	int ret;
	const char* query;
	liarcSql* sql;
	sqlite3_stmt* statement;

	ret = 1;
	sql = LISRV_OBJECT (self)->server->sql;

	/* Remove from helper tables. */
	if (!private_delete_animations (self))
		ret = 0;
	if (!private_delete_variables (self))
		ret = 0;

	/* Remove from the main table. */
	query = "DELETE FROM objects WHERE id=?;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return ret;
}

/**
 * \brief Serializes or deserializes the object.
 * 
 * \param self Object.
 * \param save Nonzero if saving, zero if reading.
 * \return Nonzero on success.
 */
int
lisrv_object_serialize (liengObject* self,
                        int          save)
{
	int ret;
	int col;
	int colgroup;
	int colmask;
	int control;
	int flags;
	int sector;
	int shape;
	float mass;
	float movement;
	float speed;
	float step;
	const char* model;
	const char* query;
	liarcSql* sql;
	limatTransform transform;
	limatVector angular;
	sqlite3_stmt* statement;

	sql = LISRV_OBJECT (self)->server->sql;

	if (!save)
	{
		/* Prepare statement. */
		query = "SELECT "
			"flags,angx,angy,angz,posx,posy,posz,rotx,roty,rotz,"
			"rotw,mass,move,speed,step,colgrp,colmsk,control,shape,model "
			"FROM objects WHERE id=?;";
		if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
			return 0;
		}
		if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Execute statement. */
		ret = sqlite3_step (statement);
		if (ret == SQLITE_DONE)
		{
			sqlite3_finalize (statement);
			return 1;
		}
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Read columns. */
		col = 0;
		flags = sqlite3_column_int (statement, col++);
		angular.x = sqlite3_column_double (statement, col++);
		angular.y = sqlite3_column_double (statement, col++);
		angular.z = sqlite3_column_double (statement, col++);
		transform.position.x = sqlite3_column_double (statement, col++);
		transform.position.y = sqlite3_column_double (statement, col++);
		transform.position.z = sqlite3_column_double (statement, col++);
		transform.rotation.x = sqlite3_column_double (statement, col++);
		transform.rotation.y = sqlite3_column_double (statement, col++);
		transform.rotation.z = sqlite3_column_double (statement, col++);
		transform.rotation.w = sqlite3_column_double (statement, col++);
		mass = sqlite3_column_double (statement, col++);
		movement = sqlite3_column_double (statement, col++);
		speed = sqlite3_column_double (statement, col++);
		step = sqlite3_column_double (statement, col++);
		colgroup = sqlite3_column_int (statement, col++);
		colmask = sqlite3_column_int (statement, col++);
		control = sqlite3_column_int (statement, col++);
		shape = sqlite3_column_int (statement, col++);
		if (sqlite3_column_type (statement, col) == SQLITE_TEXT)
			model = (char*) sqlite3_column_text (statement, col++);
		else
			model = NULL;

		/* Set state. */
		lieng_object_set_angular_momentum (self, &angular);
		lieng_object_set_transform (self, &transform);
		lieng_object_set_mass (self, mass);
		liphy_object_set_movement (self->physics, movement);
		lieng_object_set_speed (self, speed);
		lieng_object_set_collision_group (self, colgroup);
		lieng_object_set_collision_mask (self, colmask);
		liphy_object_set_control_mode (self->physics, control);
		liphy_object_set_shape_mode (self->physics, shape);
		if (model != NULL)
			lieng_object_set_model_name (self, model);
		sqlite3_finalize (statement);

		/* Read animation data. */
		if (self->script != NULL)
			private_read_animations (self);

		/* Read script data. */
		if (self->script != NULL)
			private_read_variables (self);
	}
	else
	{
		if (!private_delete_animations (self) ||
		    !private_delete_variables (self))
			return 0;
		if (lieng_object_get_realized (self))
		{
			/* Collect values. */
			flags = LISRV_OBJECT (self)->flags;
			sector = LIENG_SECTOR_INDEX (self->sector->x, self->sector->y, self->sector->z);
			model = (self->model != NULL)? self->model->name : NULL;
			mass = lieng_object_get_mass (self);
			movement = liphy_object_get_movement (self->physics);
			speed = lieng_object_get_speed (self);
			step = 0.0f;
			colgroup = lieng_object_get_collision_group (self);
			colmask = lieng_object_get_collision_mask (self);
			control = liphy_object_get_control_mode (self->physics);
			shape = liphy_object_get_shape_mode (self->physics);
			lieng_object_get_transform (self, &transform);
			lieng_object_get_angular_momentum (self, &angular);

			/* Prepare statement. */
			query = "INSERT OR REPLACE INTO objects "
				"(id,sector,flags,angx,angy,angz,posx,posy,posz,rotx,roty,rotz,"
				"rotw,mass,move,speed,step,colgrp,colmsk,control,shape,model) VALUES "
				"(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
			if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
			{
				lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
				return 0;
			}

			/* Bind values. */
			col = 1;
			ret = (sqlite3_bind_int (statement, col++, self->id) != SQLITE_OK ||
				sqlite3_bind_int (statement, col++, sector) != SQLITE_OK ||
				sqlite3_bind_int (statement, col++, flags) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, angular.x) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, angular.y) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, angular.x) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.position.x) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.position.y) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.position.z) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.rotation.x) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.rotation.y) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.rotation.z) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.rotation.w) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, mass) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, movement) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, speed) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, step) != SQLITE_OK ||
				sqlite3_bind_int (statement, col++, colgroup) != SQLITE_OK ||
				sqlite3_bind_int (statement, col++, colmask) != SQLITE_OK ||
				sqlite3_bind_int (statement, col++, control) != SQLITE_OK ||
				sqlite3_bind_int (statement, col++, shape) != SQLITE_OK);
			if (!ret)
			{
				if (model != NULL)
					ret = (sqlite3_bind_text (statement, col++, model, -1, SQLITE_TRANSIENT) != SQLITE_OK);
				else
					ret = (sqlite3_bind_null (statement, col++) != SQLITE_OK);
			}
			if (ret)
			{
				lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
				sqlite3_finalize (statement);
				return 0;
			}

			/* Write values. */
			if (sqlite3_step (statement) != SQLITE_DONE)
			{
				lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
				sqlite3_finalize (statement);
				return 0;
			}
			sqlite3_finalize (statement);

			/* Write animation data. */
			if (self->script != NULL)
				private_write_animations (self);

			/* Save script data. */
			if (self->script != NULL)
			{
				if (!private_write_variables (self))
					return 0;
			}
		}
	}

	return 1;
}

int
lisrv_object_sees (const liengObject* self,
                   const liengObject* target)
{
	lisrvObject* data = LISRV_OBJECT (self);

	if (data->client == NULL)
		return 0;
	return (lialg_u32dic_find (data->client->vision, target->id) != NULL);
}

/**
 * \brief Solves path to the requested point.
 *
 * \param self Object.
 * \param target Target position vector.
 * \return New path or NULL if couldn't solve.
 */
liaiPath*
lisrv_object_solve_path (const liengObject* self,
                         const limatVector*    target)
{
#warning Path solving is disabled.
#if 0
	liaiWaypoint* start;
	liaiWaypoint* end;
	limatVector position;
	lisrvObject* data = LISRV_OBJECT (self);

	if (self->sector == NULL || self->sector->aisector == NULL)
		return NULL;

	lisrv_object_get_position (self, &position);
	start = liai_sector_find_waypoint (self->sector->aisector, &position);
	end = liai_sector_find_waypoint (self->sector->aisector, target);
	if (start == NULL || end == NULL)
		return NULL;

	return liai_path_solver_solve (server->helper.path_solver, start, end);
#endif
	return NULL;
}

/**
 * \brief Swaps the clients of the objects.
 *
 * \param self Object.
 * \param object Object.
 */
void
lisrv_object_swap (liengObject* self,
                   liengObject* object)
{
	lisrvClient* tmp;
	lisrvObject* data = LISRV_OBJECT (self);
	lisrvObject* data1 = LISRV_OBJECT (object);

	tmp = data->client;
	lisrv_object_set_client (self, data1->client);
	lisrv_object_set_client (object, tmp);
}

/**
 * \brief Sets the client controlling the object.
 *
 * \param self Object.
 * \param value Client.
 * \return Nonzero on success.
 */
int
lisrv_object_set_client (liengObject* self,
                         lisrvClient* value)
{
	lisrvObject* data = LISRV_OBJECT (self);

	if (data->client == value)
		return 1;
	data->client = value;
	if (value != NULL)
	{
		lieng_object_ref (self, 1);
		return lisrv_client_set_object (value, self);
	}
	else
	{
		lieng_object_ref (self, -1);
		return 1;
	}
}

/**
 * \brief Gets the model number of the object.
 *
 * \param self An object.
 * \return Model number.
 */
uint32_t
lisrv_object_get_model (const liengObject* self)
{
	return lieng_object_get_model_code (self);
}

/**
 * \brief Sets the model of an object.
 *
 * \param self Object.
 * \param value Model.
 * \return Nonzero on success.
 */
int
lisrv_object_set_model (liengObject* self,
                        liengModel*  value)
{
	if (!lieng_default_calls.lieng_object_set_model (self, value))
		return 0;

	/* Invoke callbacks. */
	lieng_engine_call (self->engine, LISRV_CALLBACK_OBJECT_MODEL, self, value);

	return 1;
}

/**
 * \brief Gets the name of the object.
 *
 * \param self Object.
 * \return String guaranteed to not be NULL.
 */
const char*
lisrv_object_get_name (const liengObject* self)
{
	lisrvObject* data = LISRV_OBJECT (self);

	if (data->name == NULL)
		return "";
	return data->name;
}

/**
 * \brief Sets the name of the object.
 *
 * \param self Object.
 * \param value String.
 * \return Nonzero if successsful.
 */
int
lisrv_object_set_name (liengObject* self,
                       const char*  value)
{
	char* tmp;
	lisrvObject* data = LISRV_OBJECT (self);

	tmp = strdup (value);
	if (tmp == NULL)
		return 0;
	free (data->name);
	data->name = tmp;

	return 1;
}

/**
 * \brief Sets the world transformation of the object.
 *
 * This function takes care of moving the object between map sectors and
 * sending events. If you want to modify to the position of an object,
 * this function is basically the only valid way of doing it.
 *
 * If the object is not realized, the position is set but nothing else
 * happens. However, if the object is later realized, the position set
 * with this function will be used as its world position.
 *
 * \param self Object.
 * \param value Transformation.
 * \return Nonzero on success.
 */
int
lisrv_object_set_transform (liengObject*          self,
                            const limatTransform* value)
{
	/* Call base. */
	if (!lieng_default_calls.lieng_object_set_transform (self, value))
		return 0;

	return 1;
}

int
lisrv_object_set_realized (liengObject* self,
                           int          value)
{
	if (value == lieng_object_get_realized (self))
		return 1;
	if (value)
	{
		/* Call base. */
		if (!lieng_default_calls.lieng_object_set_realized (self, 1))
			return 0;

		/* Invoke callbacks. */
		lieng_engine_call (self->engine, LISRV_CALLBACK_OBJECT_VISIBILITY, self, 1);
	}
	else
	{
		/* Invoke callbacks. */
		lieng_engine_call (self->engine, LISRV_CALLBACK_OBJECT_VISIBILITY, self, 0);

		/* Call base. */
		if (!lieng_default_calls.lieng_object_set_realized (self, 0))
			return 0;
	}

	return 1;
}

int
lisrv_object_get_valid (const liengObject* self)
{
	return !self->script->invalid;
}

/**
 * \brief Set the velocity vector of the object.
 *
 * \param self object
 * \param value Vector.
 * \return Nonzero on success.
 */
int
lisrv_object_set_velocity (liengObject*       self,
                           const limatVector* value)
{
	/* Call base. */
	if (!lieng_default_calls.lieng_object_set_velocity (self, value))
		return 0;

	return 1;
}

/*****************************************************************************/

static int
private_delete_animations (liengObject* self)
{
	const char* query;
	liarcSql* sql;
	sqlite3_stmt* statement;

	sql = LISRV_OBJECT (self)->server->sql;

	/* Clear animation data. */
	query = "DELETE from object_anims WHERE id=?;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return 1;
}

static int
private_delete_variables (liengObject* self)
{
	const char* query;
	liarcSql* sql;
	sqlite3_stmt* statement;

	sql = LISRV_OBJECT (self)->server->sql;

	/* Clear script data. */
	query = "DELETE from object_vars WHERE id=?;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return 1;
}

static int
private_read_animations (liengObject* self)
{
	int ret;
	int chan;
	float prio;
	const char* name;
	const char* query;
	liarcSql* sql;
	sqlite3_stmt* statement;

	sql = LISRV_OBJECT (self)->server->sql;

	/* Prepare statement. */
	query = "SELECT name,chan,prio FROM object_anims WHERE id=?;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}

	/* Execute statement. */
	while (1)
	{
		ret = sqlite3_step (statement);
		if (ret == SQLITE_DONE)
		{
			sqlite3_finalize (statement);
			break;
		}
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}
		name = (char*) sqlite3_column_text (statement, 0);
		chan = sqlite3_column_int (statement, 1);
		prio = sqlite3_column_double (statement, 2);
		if (name != NULL && *name != '\0' && chan >= 0 && prio > 0.0f)
			lisrv_object_animate (self, name, chan, prio, 1);
	}

	return 1;
}

static int
private_read_variables (liengObject* self)
{
	int ret;
	const char* query;
	liarcSql* sql;
	liscrScript* script;
	sqlite3_stmt* statement;

	sql = LISRV_OBJECT (self)->server->sql;
	script = LISRV_OBJECT (self)->server->script;

	/* Get custom deserialization function. */
	liscr_pushdata (script->lua, self->script);
	lua_pushstring (script->lua, "loaded");
	lua_gettable (script->lua, -2);
	if (lua_type (script->lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (script->lua, 2);
		return 1;
	}

	/* Prepare statement. */
	query = "SELECT name,value FROM object_vars WHERE id=?;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		lua_pop (script->lua, 1);
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		lua_pop (script->lua, 1);
		return 0;
	}

	/* Execute statement. */
	liscr_pushdata (script->lua, self->script);
	lua_newtable (script->lua);
	while (1)
	{
		ret = sqlite3_step (statement);
		if (ret == SQLITE_DONE)
		{
			sqlite3_finalize (statement);
			break;
		}
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			lua_pop (script->lua, 3);
			return 0;
		}
		if (sqlite3_column_type (statement, 0) != SQLITE_TEXT)
			continue;
		switch (sqlite3_column_type (statement, 1))
		{
			case SQLITE_TEXT:
				lua_pushstring (script->lua, (char*) sqlite3_column_text (statement, 0));
				lua_pushstring (script->lua, (char*) sqlite3_column_text (statement, 1));
				lua_settable (script->lua, -3);
				break;
			case SQLITE_INTEGER:
				lua_pushstring (script->lua, (char*) sqlite3_column_text (statement, 0));
				lua_pushnumber (script->lua, sqlite3_column_int (statement, 1));
				lua_settable (script->lua, -3);
				break;
			case SQLITE_FLOAT:
				lua_pushstring (script->lua, (char*) sqlite3_column_text (statement, 0));
				lua_pushnumber (script->lua, sqlite3_column_double (statement, 1));
				lua_settable (script->lua, -3);
				break;
		}
	}

	/* Deserialize fields. */
	if (lua_pcall (script->lua, 2, 0, 0) != 0)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "%s", lua_tostring (script->lua, -1));
		lua_pop (script->lua, 1);
		return 0;
	}

	return 1;
}

static int
private_write_animations (liengObject* self)
{
	const char* query;
	lialgU32dicIter iter;
	liarcSql* sql;
	lisrvAniminfo* info;
	sqlite3_stmt* statement;

	sql = LISRV_OBJECT (self)->server->sql;

	/* Save all permanent animations. */
	LI_FOREACH_U32DIC (iter, LISRV_OBJECT (self)->animations)
	{
		info = iter.value;
		if (!info->permanent)
			continue;
		assert (info->animation != NULL);

		/* Prepare statement. */
		query = "INSERT INTO object_anims (id,name,chan,prio) VALUES (?,?,?,?);";
		if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
			return 0;
		}

		/* Bind values. */
		if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK ||
		    sqlite3_bind_text (statement, 2, info->animation->name, -1, SQLITE_TRANSIENT) != SQLITE_OK ||
		    sqlite3_bind_int (statement, 3, info->channel) != SQLITE_OK ||
		    sqlite3_bind_double (statement, 4, info->priority) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Execute statement. */
		if (sqlite3_step (statement) != SQLITE_DONE)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}
		sqlite3_finalize (statement);
	}

	return 1;
}

static int
private_write_variables (liengObject* self)
{
	int ret;
	double value_num;
	const char* value_str;
	const char* key;
	const char* query;
	liarcSql* sql;
	liscrScript* script;
	sqlite3_stmt* statement;

	sql = LISRV_OBJECT (self)->server->sql;
	script = LISRV_OBJECT (self)->server->script;

	/* Get custom serialization function. */
	liscr_pushdata (script->lua, self->script);
	lua_pushstring (script->lua, "saved");
	lua_gettable (script->lua, -2);
	if (lua_type (script->lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (script->lua, 2);
		return 1;
	}

	/* Get custom serialization fields. */
	lua_pushvalue (script->lua, -2);
	lua_remove (script->lua, -3);
	if (lua_pcall (script->lua, 1, 1, 0) != 0)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "%s", lua_tostring (script->lua, -1));
		lua_pop (script->lua, 1);
		return 0;
	}
	if (lua_type (script->lua, -1) != LUA_TTABLE)
	{
		lua_pop (script->lua, 1);
		return 1;
	}

	/* Insert fields to the database. */
	lua_pushnil (script->lua);
	while (lua_next (script->lua, -2) != 0)
	{
		/* Check for valid types. */
		if (lua_type (script->lua, -2) != LUA_TSTRING ||
		   (lua_type (script->lua, -1) != LUA_TSTRING &&
		    lua_type (script->lua, -1) != LUA_TNUMBER))
		{
			lua_pop (script->lua, 1);
			continue;
		}
		key = lua_tostring (script->lua, -2);

		/* Prepare statement. */
		query = "INSERT INTO object_vars (id,name,value) VALUES (?,?,?);";
		if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
			lua_pop (script->lua, 3);
			return 0;
		}

		/* Bind values. */
		if (lua_type (script->lua, -1) == LUA_TSTRING)
		{
			value_str = lua_tostring (script->lua, -1);
			ret = (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK ||
			       sqlite3_bind_text (statement, 2, key, -1, SQLITE_TRANSIENT) != SQLITE_OK ||
			       sqlite3_bind_text (statement, 3, value_str, -1, SQLITE_TRANSIENT) != SQLITE_OK);
		}
		else
		{
			value_num = lua_tonumber (script->lua, -1);
			ret = (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK ||
			       sqlite3_bind_text (statement, 2, key, -1, SQLITE_TRANSIENT) != SQLITE_OK ||
			       sqlite3_bind_double (statement, 3, value_num) != SQLITE_OK);
		}
		if (ret)
		{
			lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			lua_pop (script->lua, 3);
			return 0;
		}

		/* Execute statement. */
		if (sqlite3_step (statement) != SQLITE_DONE)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			lua_pop (script->lua, 3);
			return 0;
		}
		sqlite3_finalize (statement);
		lua_pop (script->lua, 1);
	}
	lua_pop (script->lua, 1);

	return 1;
}

/** @} */
/** @} */

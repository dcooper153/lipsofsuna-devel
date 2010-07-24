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
 * \addtogroup LISerServer Server
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/script.h>
#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "server.h"
#include "server-callbacks.h"
#include "server-script.h"

static int
private_init_sql (LISerServer* self);

/****************************************************************************/

/**
 * \brief Creates a new server instance.
 *
 * \param paths Path information.
 * \param args Arguments passed to the program.
 * \return New server or NULL.
 */
LISerServer* liser_server_new (
	LIPthPaths* paths,
	const char* args)
{
	LISerServer* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LISerServer));
	if (self == NULL)
		return NULL;

	/* Create program. */
	self->program = limai_program_new (paths, args);
	if (self->program == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->program->sleep = 5000;
	self->sectors = self->program->sectors;
	self->callbacks = self->program->callbacks;
	self->engine = self->program->engine;
	self->paths = self->program->paths;
	self->script = self->program->script;
	lieng_engine_set_local_range (self->engine, LINET_RANGE_SERVER_START, LINET_RANGE_SERVER_END);
	lieng_engine_set_unique_object_call (self->engine, liser_server_check_unique_object, self);
	lialg_sectors_set_unload (self->sectors, 20.0f);

	/* Initialize server component. */
	if (!limai_program_insert_component (self->program, "server", self))
	{
		limai_program_free (self->program);
		return NULL;
	}
	if (!private_init_sql (self) ||
	    !liser_server_init_callbacks_client (self) ||
	    !liscr_script_create_class (self->script, "Class", liscr_script_class, self->script) ||
	    !liscr_script_create_class (self->script, "Data", liscr_script_data, self->script) ||
	    !liscr_script_create_class (self->script, "Event", liscr_script_event, self->script) ||
	    !liscr_script_create_class (self->script, "Object", liser_script_object, self->program) ||
	    !liscr_script_create_class (self->script, "Packet", liscr_script_packet, self->script) ||
	    !liscr_script_create_class (self->script, "Path", liscr_script_path, self->script) ||
	    !liscr_script_create_class (self->script, "Program", liscr_script_program, self->program) ||
	    !liscr_script_create_class (self->script, "Quaternion", liscr_script_quaternion, self->script) ||
	    !liscr_script_create_class (self->script, "Server", liser_script_server, self) ||
	    !liscr_script_create_class (self->script, "Vector", liscr_script_vector, self->script))
	{
		liser_server_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the server.
 *
 * \param self Server.
 */
void
liser_server_free (LISerServer* self)
{
	/* Invoke callbacks. */
	if (self->callbacks != NULL)
		lical_callbacks_call (self->callbacks, self, "server-free", lical_marshal_DATA);

	/* Free program. */
	if (self->program != NULL)
	{
		limai_program_remove_component (self->program, "server");
		limai_program_free (self->program);
	}

	/* Free database. */
	if (self->sql != NULL)
		sqlite3_close (self->sql);

	lisys_free (self);
}

/**
 * \brief Checks if the object ID is unique.
 *
 * \param self Server.
 * \param id ID to check.
 * \return Nonzero if unique.
 */
int
liser_server_check_unique_object (const LISerServer* self,
                                  uint32_t           id)
{
	int ret;
	const char* query;
	sqlite3_stmt* statement;

	/* Query the ID from database. */
	query = "SELECT id FROM objects WHERE id=?;";
	if (sqlite3_prepare_v2 (self->sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (self->sql));
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self->sql));
		sqlite3_finalize (statement);
		return 0;
	}

	/* Reject if found. */
	ret = sqlite3_step (statement);
	if (ret != SQLITE_DONE)
	{
		if (ret != SQLITE_ROW)
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self->sql));
		ret = 0;
	}
	else
		ret = 1;
	sqlite3_finalize (statement);

	return ret;
}

/**
 * \brief Runs the server script.
 *
 * \param self Server.
 * \return Nonzero on success.
 */
int
liser_server_main (LISerServer* self)
{
	return limai_program_execute_script (self->program, "server/main.lua");
}

/**
 * \brief Saves currently loaded objects.
 *
 * \param self Server.
 * \return Nonzero on success.
 */
int
liser_server_save (LISerServer* self)
{
	int ret;
	LIAlgU32dicIter iter;
	LIEngObject* object;

	ret = 1;
	LIALG_U32DIC_FOREACH (iter, self->engine->objects)
	{
		object = iter.value;
		if (object->flags & LIENG_OBJECT_FLAG_SAVE)
		{
			if (!liser_object_serialize (object, self, 1))
				ret = 0;
		}
	}

	return ret;
}

/****************************************************************************/

static int
private_init_sql (LISerServer* self)
{
	int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	char* path;
	const char* query;
	sqlite3_stmt* statement;

	/* Format path. */
	path = lipth_paths_get_sql (self->paths, "server.db");
	if (path == NULL)
		return 0;

	/* Open database. */
	if (sqlite3_open_v2 (path, &self->sql, flags, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "sqlite: %s", sqlite3_errmsg (self->sql));
		sqlite3_close (self->sql);
		lisys_free (path);
		return 0;
	}
	lisys_free (path);

	/* Create info table. */
	query = "CREATE TABLE IF NOT EXISTS info (version TEXT);";
	if (sqlite3_prepare_v2 (self->sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "sqlite: %s", sqlite3_errmsg (self->sql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Create object table. */
	query = "CREATE TABLE IF NOT EXISTS objects "
		"(id INTEGER PRIMARY KEY,sector UNSIGNED INTEGER,model TEXT,"
		"flags UNSIGNED INTEGER,angx REAL,angy REAL,angz REAL,posx REAL,"
		"posy REAL,posz REAL,rotx REAL,roty REAL,rotz REAL,rotw REAL,"
		"mass REAL,move REAL,speed REAL,step REAL,colgrp UNSIGNED INTEGER,"
		"colmsk UNSIGNED INTEGER,control UNSIGNED INTEGER,type TEXT,extra TEXT);";
	if (sqlite3_prepare_v2 (self->sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "sqlite: %s", sqlite3_errmsg (self->sql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return 1;
}

/** @} */
/** @} */

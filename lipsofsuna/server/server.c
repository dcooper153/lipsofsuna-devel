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
#include "server-client.h"
#include "server-extension.h"
#include "server-script.h"

#define LI_SERVER_SLEEP

static int
private_init_engine (LISerServer* self);

static int
private_init_extensions (LISerServer* self);

static int
private_init_script (LISerServer* self);

static int
private_init_sql (LISerServer* self);

static int
private_init_time (LISerServer* self);

/****************************************************************************/

/**
 * \brief Creates a new server instance.
 *
 * \param paths Path information.
 * \return New server or NULL.
 */
LISerServer*
liser_server_new (LIPthPaths* paths)
{
	LISerServer* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LISerServer));
	if (self == NULL)
		return NULL;
	self->paths = paths;

	/* Initialize subsystems. */
	if (!private_init_sql (self) ||
	    !private_init_extensions (self) ||
	    !private_init_engine (self) ||
	    !private_init_time (self) ||
	    !private_init_script (self))
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
	LIAlgStrdicIter iter;
	LISerExtension* extension;

	/* Free script. */
	if (self->script != NULL)
		liscr_script_free (self->script);

	/* Free extensions. */
	if (self->extensions != NULL)
	{
		LIALG_STRDIC_FOREACH (iter, self->extensions)
		{
			extension = iter.value;
			((void (*)(void*)) extension->info->free) (extension->object);
			lisys_module_free (extension->module);
			lisys_free (extension);
		}
		lialg_strdic_free (self->extensions);
	}

	/* Free networking. */
	if (self->network != NULL)
		liser_network_free (self->network);

	/* Free engine. */
	if (self->engine != NULL)
		lieng_engine_free (self->engine);

	/* Free database. */
	if (self->sql != NULL)
		sqlite3_close (self->sql);

	/* Free callbacks. */
	if (self->callbacks != NULL)
		lical_callbacks_free (self->callbacks);

	lisys_free (self);
}

/**
 * \brief Finds an extension by name.
 *
 * \param self Server.
 * \param name Extension name.
 * \return Extension or NULL.
 */
LISerExtension*
liser_server_find_extension (LISerServer* self,
                             const char*  name)
{
	return lialg_strdic_find (self->extensions, name);
}

/**
 * \brief Loads an extension.
 *
 * \param self Server.
 * \param name Extensions name.
 * \return Nonzero on success.
 */
int
liser_server_load_extension (LISerServer* self,
                             const char*  name)
{
	char* path;
	LISysModule* module;
	LISerExtension* extension;
	LISerExtensionInfo* info;

	/* Check if already loaded. */
	module = lialg_strdic_find (self->extensions, name);
	if (module != NULL)
		return 1;

	/* Construct full path. */
	path = lisys_path_format (
		self->paths->global_exts, LISYS_PATH_SEPARATOR,
		"lib", name, "-ser.", LISYS_EXTENSION_DLL, NULL);
	if (path == NULL)
		return 0;

	/* Open module file. */
	module = lisys_module_new (path, 0);
	lisys_free (path);
	if (module == NULL)
		goto error;

	/* Find module info. */
	info = lisys_module_symbol (module, "liextInfo");
	if (info == NULL)
	{
		lisys_error_set (EINVAL, "no module info");
		lisys_module_free (module);
		goto error;
	}
	if (info->version != LISER_EXTENSION_VERSION)
	{
		lisys_error_set (EINVAL, "invalid module version");
		lisys_module_free (module);
		goto error;
	}
	if (info->name == NULL || info->init == NULL || info->free == NULL)
	{
		lisys_error_set (EINVAL, "invalid module format");
		lisys_module_free (module);
		goto error;
	}

	/* Insert to extension list. */
	extension = lisys_calloc (1, sizeof (LISerExtension));
	if (extension == NULL)
		goto error;
	extension->info = info;
	extension->module = module;
	if (extension == NULL)
	{
		lisys_module_free (module);
		goto error;
	}
	if (!lialg_strdic_insert (self->extensions, name, extension))
	{
		lisys_module_free (module);
		lisys_free (extension);
		goto error;
	}

	/* Call module initializer. */
	extension->object = ((void* (*)(LISerServer*)) info->init)(self);
	if (extension->object == NULL)
	{
		lialg_strdic_remove (self->extensions, name);
		lisys_module_free (module);
		lisys_free (extension);
		goto error;
	}

	return 1;

error:
	lisys_error_append ("cannot initialize module `%s'", name);
	return 0;
}

/**
 * \brief Runs the server in a loop until it exits.
 *
 * \param self Server.
 * \return Nonzero on success.
 */
int
liser_server_main (LISerServer* self)
{
	float secs;
	struct timeval curr_tick;
	struct timeval prev_tick;

	/* Main loop. */
	gettimeofday (&prev_tick, NULL);
	while (1)
	{
		gettimeofday (&curr_tick, NULL);
		secs = curr_tick.tv_sec - prev_tick.tv_sec +
			  (curr_tick.tv_usec - prev_tick.tv_usec) * 0.000001;
		prev_tick = curr_tick;
		if (!liser_server_update (self, secs))
			break;
#ifdef LI_SERVER_SLEEP
		lisys_usleep (5000);
#endif
	}

	return 1;
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
			if (!liser_object_serialize (object, 1))
				ret = 0;
		}
	}

	return ret;
}

/**
 * \brief Tells the server to shut down.
 *
 * \note Thread safe.
 * \param self Server.
 */
void
liser_server_shutdown (LISerServer* self)
{
	self->quit = 1;
}

/**
 * \brief Updates the server state.
 *
 * \param self Server.
 * \param secs Duration of the tick in seconds.
 * \return Nonzero if the server is still running.
 */
int
liser_server_update (LISerServer* self,
                     float        secs)
{
	if (self->quit)
		return 0;

	liscr_script_update (self->script, secs);
	lieng_engine_update (self->engine, secs);
	if (self->network != NULL)
		liser_network_update (self->network, secs);
	lical_callbacks_call (self->callbacks, self->engine, "tick", lical_marshal_DATA_FLT, secs);

	return !self->quit;
}

/**
 * \brief Gets time since server startup.
 *
 * \param self Server.
 * \return Time in seconds.
 */
double
liser_server_get_time (const LISerServer* self)
{
	struct timeval t;

	gettimeofday (&t, NULL);
	t.tv_sec -= self->time.start.tv_sec;
	t.tv_usec -= self->time.start.tv_usec;
	if (t.tv_usec < 0)
	{
		t.tv_sec -= 1;
		t.tv_usec += 1000000;
	}

	return (double) t.tv_sec + (double) t.tv_usec * 0.000001;
}

/**
 * \brief Returns a unique object ID.
 *
 * \param self Server.
 * \return Unique object ID or zero on error.
 */
uint32_t
liser_server_get_unique_object (const LISerServer* self)
{
	int ret;
	uint32_t id;
	const char* query;
	sqlite3_stmt* statement;

	for (id = 0 ; !id ; )
	{
		/* Choose random number. */
		id = self->engine->range.start + lisys_randi (self->engine->range.size - 1);
		if (!id)
			continue;

		/* Reject numbers of loaded objects. */
		if (lialg_u32dic_find (self->engine->objects, id))
		{
			id = 0;
			continue;
		}

		/* Reject numbers of database objects. */
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
		ret = sqlite3_step (statement);
		if (ret != SQLITE_DONE)
		{
			if (ret != SQLITE_ROW)
			{
				lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self->sql));
				sqlite3_finalize (statement);
				return 0;
			}
			id = 0;
		}
		sqlite3_finalize (statement);
	}

	return id;
}

/****************************************************************************/

static int
private_init_engine (LISerServer* self)
{
	/* Initialize callbacks. */
	self->callbacks = lical_callbacks_new ();
	if (self->callbacks == NULL)
		return 0;

	/* Initialize sector. */
#warning Hardcoded sector size
	self->sectors = lialg_sectors_new (256, 64.0f);
	if (self->sectors == NULL)
		return 0;

	/* Create engine. */
	self->engine = lieng_engine_new (self->callbacks, self->sectors, self->paths->module_data);
	if (self->engine == NULL)
		return 0;
	lieng_engine_set_local_range (self->engine, LINET_RANGE_SERVER_START, LINET_RANGE_SERVER_END);
	lieng_engine_set_userdata (self->engine, self);

	/* Connect callbacks. */
	if (!liser_server_init_callbacks_client (self))
		return 0;

	/* Load resources. */
	if (!lieng_engine_load_resources (self->engine, NULL))
		return 0;

	return 1;
}

static int
private_init_extensions (LISerServer* self)
{
	self->extensions = lialg_strdic_new ();
	if (self->extensions == NULL)
		return 0;
	return 1;
}

/**
 * \brief Loads the scripts and objects.
 *
 * \param self Server.
 * \return Nonzero on success.
 */
static int
private_init_script (LISerServer* self)
{
	int ret;
	char* path;

	/* Allocate the script. */
	self->script = liscr_script_new ();
	if (self->script == NULL)
		return 0;
	liscr_script_set_userdata (self->script, self);

	/* Register classes. */
	if (!liscr_script_create_class (self->script, "Class", liscr_script_class, self->script) ||
	    !liscr_script_create_class (self->script, "Effect", liser_script_effect, self) ||
	    !liscr_script_create_class (self->script, "Extension", liser_script_extension, self) ||
	    !liscr_script_create_class (self->script, "Object", liser_script_objet, self) ||
	    !liscr_script_create_class (self->script, "Packet", liscr_script_packet, self->script) ||
	    !liscr_script_create_class (self->script, "Path", liscr_script_path, self->script) ||
	    !liscr_script_create_class (self->script, "Quaternion", liscr_script_quaternion, self->script) ||
	    !liscr_script_create_class (self->script, "Server", liser_script_server, self) ||
	    !liscr_script_create_class (self->script, "Vector", liscr_script_vector, self->script))
		return 0;

	/* Load the script. */
	path = lipth_paths_get_script (self->paths, "server/main.lua");
	if (path == NULL)
		return 0;
	ret = liscr_script_load (self->script, path);
	lisys_free (path);
	if (!ret)
		return 0;

	return 1;
}

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

	/* Create object animation table. */
	query = "CREATE TABLE IF NOT EXISTS object_anims "
		"(id UNSIGNED INTEGER REFERENCES objects(id),name TEXT,chan REAL,prio REAL);";
	if (sqlite3_prepare_v2 (self->sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "sqlite: %s", sqlite3_errmsg (self->sql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "sqlite: %s", sqlite3_errmsg (self->sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return 1;
}

static int
private_init_time (LISerServer* self)
{
	gettimeofday (&self->time.start, NULL);
	return 1;
}

/** @} */
/** @} */

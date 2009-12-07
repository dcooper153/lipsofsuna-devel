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
 * \addtogroup lisrvServer Server
 * @{
 */

#include <config/lips-config.h>
#include <network/lips-network.h>
#include <script/lips-script.h>
#include <string/lips-string.h>
#include <system/lips-system.h>
#include "server.h"
#include "server-callbacks.h"
#include "server-client.h"
#include "server-extension.h"
#include "server-script.h"

#define LI_SERVER_SLEEP

static int
private_init_ai (lisrvServer* self);

static int
private_init_bans (lisrvServer* self);

static int
private_init_engine (lisrvServer* self);

static int
private_init_extensions (lisrvServer* self);

static int
private_init_host (lisrvServer* self);

static int
private_init_script (lisrvServer* self);

static int
private_init_sql (lisrvServer* self);

static int
private_init_time (lisrvServer* self);

/****************************************************************************/

/**
 * \brief Creates a new server instance.
 *
 * \param paths Path information.
 * \return New server or NULL.
 */
lisrvServer*
lisrv_server_new (lipthPaths* paths)
{
	lisrvServer* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lisrvServer));
	if (self == NULL)
		return NULL;
	self->paths = paths;

	/* Initialize mutexes. */
	pthread_mutex_init (&self->mutexes.bans, NULL);

	/* Initialize subsystems. */
	if (!private_init_sql (self) ||
	    !private_init_ai (self) ||
	    !private_init_host (self) ||
	    !private_init_bans (self) ||
	    !private_init_extensions (self) ||
	    !private_init_engine (self) ||
	    !private_init_time (self) ||
	    !private_init_script (self))
	{
		lisrv_server_free (self);
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
lisrv_server_free (lisrvServer* self)
{
	lialgStrdicIter iter;
	lisrvExtension* extension;

	/* Free script. */
	if (self->script != NULL)
		liscr_script_free (self->script);

	/* Free extensions. */
	if (self->extensions != NULL)
	{
		LI_FOREACH_STRDIC (iter, self->extensions)
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
		lisrv_network_free (self->network);

	/* Free engine. */
	if (self->engine != NULL)
		lieng_engine_free (self->engine);

	/* Free database. */
	if (self->sql != NULL)
		sqlite3_close (self->sql);

	/* Free mutexes. */
	pthread_mutex_destroy (&self->mutexes.bans);

	/* Free configuration. */
	if (self->config.bans != NULL)
		licfg_bans_free (self->config.bans);
	if (self->config.host != NULL)
		licfg_host_free (self->config.host);

	/* Free helpers. */
	if (self->helper.path_solver != NULL)
		liai_path_solver_free (self->helper.path_solver);
	if (self->helper.resources != NULL)
		liarc_writer_free (self->helper.resources);

	lisys_free (self);
}

/**
 * \brief Loads an extension.
 *
 * \param self Server.
 * \param name Extensions name.
 * \return Nonzero on success.
 */
int
lisrv_server_load_extension (lisrvServer* self,
                             const char*  name)
{
	char* path;
	lisysModule* module;
	lisrvExtension* extension;
	lisrvExtensionInfo* info;

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
	if (info->version != LISRV_EXTENSION_VERSION)
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
	extension = lisys_calloc (1, sizeof (lisrvExtension));
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
	extension->object = ((void* (*)(lisrvServer*)) info->init)(self);
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
 * \brief Inserts a ban rule.
 *
 * \param self Server.
 * \param ip Address to ban.
 * \return Nonzero on success.
 */
int
lisrv_server_insert_ban (lisrvServer* self,
                         const char*  ip)
{
	int ret;

	pthread_mutex_lock (&self->mutexes.bans);
	ret = licfg_bans_insert_ban (self->config.bans, ip);
	pthread_mutex_unlock (&self->mutexes.bans);
	return ret;
}

/**
 * \brief Runs the server in a loop until it exits.
 *
 * \param self Server.
 * \return Nonzero on success.
 */
int
lisrv_server_main (lisrvServer* self)
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
		if (!lisrv_server_update (self, secs))
			break;
#ifdef LI_SERVER_SLEEP
		lisys_usleep (5000);
#endif
	}

	return 1;
}

/**
 * \brief Removes a ban rule.
 *
 * \param self Server.
 * \param ip Address to unban.
 * \return Nonzero on success.
 */
int
lisrv_server_remove_ban (lisrvServer* self,
                         const char*  ip)
{
	int ret;

	pthread_mutex_lock (&self->mutexes.bans);
	ret = licfg_bans_remove_ban (self->config.bans, ip);
	pthread_mutex_unlock (&self->mutexes.bans);
	return ret;
}

/**
 * \brief Saves currently loaded objects.
 *
 * \param self Server.
 * \return Nonzero on success.
 */
int
lisrv_server_save (lisrvServer* self)
{
	int ret;
	lialgU32dicIter iter;
	liengObject* object;

	ret = 1;
	LI_FOREACH_U32DIC (iter, self->engine->objects)
	{
		object = iter.value;
		if (object->flags & LIENG_OBJECT_FLAG_SAVE)
		{
			if (!lisrv_object_serialize (object, 1))
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
lisrv_server_shutdown (lisrvServer* self)
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
lisrv_server_update (lisrvServer* self,
                     float        secs)
{
	if (self->quit)
		return 0;

	liscr_script_update (self->script, secs);
	lieng_engine_update (self->engine, secs);
	lisrv_network_update (self->network, secs);
	lieng_engine_call (self->engine, LISRV_CALLBACK_TICK, secs);

	return !self->quit;
}

/**
 * \brief Checks if the address is banned.
 *
 * \param self Server.
 * \param address Address string.
 * \return Nonzero if the socket is banned.
 */
int
lisrv_server_get_banned (lisrvServer* self,
                         const char*  address)
{
	return licfg_bans_get_banned (self->config.bans, address);
}

/**
 * \brief Gets time since server startup.
 *
 * \param self Server.
 * \return Time in seconds.
 */
double
lisrv_server_get_time (const lisrvServer* self)
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
lisrv_server_get_unique_object (const lisrvServer* self)
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
private_init_ai (lisrvServer* self)
{
	self->helper.path_solver = liai_path_solver_new ();
	if (self->helper.path_solver == NULL)
		return 0;
	return 1;
}

static int
private_init_bans (lisrvServer* self)
{
	self->config.bans = licfg_bans_new_from_file (self->paths->module_state);
	if (self->config.bans == NULL)
	{
		if (lisys_error_peek () != EIO)
			return 0;
		self->config.bans = licfg_bans_new ();
		if (self->config.bans == NULL)
			return 0;
		lisys_error_get (NULL);
		return 1;
	}
	return 1;
}

static int
private_init_engine (lisrvServer* self)
{
	int i;
	liengAnimation* anim;
	liengModel* model;
	liengSample* sample;

	/* Create engine. */
	self->engine = lieng_engine_new (self->paths->module_data);
	if (self->engine == NULL)
		return 0;
	lieng_engine_set_local_range (self->engine, LINET_RANGE_SERVER_START, LINET_RANGE_SERVER_END);
	lieng_engine_set_userdata (self->engine, LIENG_DATA_SERVER, self);

	/* Initialize callbacks. */
	if (!lical_callbacks_insert_type (self->engine->callbacks, LISRV_CALLBACK_CLIENT_CONTROL, lical_marshal_DATA_PTR_PTR_INT) ||
	    !lical_callbacks_insert_type (self->engine->callbacks, LISRV_CALLBACK_CLIENT_LOGIN, lical_marshal_DATA_PTR_PTR_PTR) ||
	    !lical_callbacks_insert_type (self->engine->callbacks, LISRV_CALLBACK_CLIENT_LOGOUT, lical_marshal_DATA_PTR) ||
	    !lical_callbacks_insert_type (self->engine->callbacks, LISRV_CALLBACK_CLIENT_PACKET, lical_marshal_DATA_PTR_PTR) ||
	    !lical_callbacks_insert_type (self->engine->callbacks, LISRV_CALLBACK_TICK, lical_marshal_DATA_FLT) ||
	    !lical_callbacks_insert_type (self->engine->callbacks, LISRV_CALLBACK_OBJECT_CLIENT, lical_marshal_DATA_PTR) ||
	    !lical_callbacks_insert_type (self->engine->callbacks, LISRV_CALLBACK_OBJECT_ANIMATION, lical_marshal_DATA_PTR_PTR) ||
	    !lical_callbacks_insert_type (self->engine->callbacks, LISRV_CALLBACK_OBJECT_SAMPLE, lical_marshal_DATA_PTR_PTR_INT) ||
	    !lical_callbacks_insert_type (self->engine->callbacks, LISRV_CALLBACK_VISION_HIDE, lical_marshal_DATA_PTR_PTR) ||
	    !lical_callbacks_insert_type (self->engine->callbacks, LISRV_CALLBACK_VISION_SHOW, lical_marshal_DATA_PTR_PTR))
		return 0;

	/* Load resources. */
	if (!lieng_engine_load_resources (self->engine, NULL))
		return 0;

	/* Build resource packet. */
	/* TODO: Should use compression. */
	self->helper.resources = liarc_writer_new_packet (LINET_SERVER_PACKET_RESOURCES);
	if (self->helper.resources == NULL)
		return 0;
	liarc_writer_append_uint32 (self->helper.resources, self->engine->resources->animations.count);
	liarc_writer_append_uint32 (self->helper.resources, self->engine->resources->models.count);
	liarc_writer_append_uint32 (self->helper.resources, self->engine->resources->samples.count);
	for (i = 0 ; i < self->engine->resources->animations.count ; i++)
	{
		anim = lieng_resources_find_animation_by_code (self->engine->resources, i);
		assert (anim != NULL);
		liarc_writer_append_string (self->helper.resources, anim->name);
		liarc_writer_append_nul (self->helper.resources);
	}
	for (i = 0 ; i < self->engine->resources->models.count ; i++)
	{
		model = lieng_resources_find_model_by_code (self->engine->resources, i);
		assert (model != NULL);
		liarc_writer_append_string (self->helper.resources, model->name);
		liarc_writer_append_nul (self->helper.resources);
	}
	for (i = 0 ; i < self->engine->resources->samples.count ; i++)
	{
		sample = lieng_resources_find_sample_by_code (self->engine->resources, i);
		assert (sample != NULL);
		liarc_writer_append_string (self->helper.resources, sample->name);
		liarc_writer_append_nul (self->helper.resources);
	}

	return 1;
}

static int
private_init_extensions (lisrvServer* self)
{
	self->extensions = lialg_strdic_new ();
	if (self->extensions == NULL)
		return 0;
	return 1;
}

static int
private_init_host (lisrvServer* self)
{
	/* Read host configuration. */
	self->config.host = licfg_host_new (self->paths->module_data);
	if (self->config.host == NULL)
		return 0;

	/* Initialize networking. */
	self->network = lisrv_network_new (self, self->config.host->udp, self->config.host->port);
	if (self->network == NULL)
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
private_init_script (lisrvServer* self)
{
	int ret;
	char* path;

	/* Allocate the script. */
	self->script = liscr_script_new ();
	if (self->script == NULL)
		return 0;
	liscr_script_set_userdata (self->script, self);

	/* Register classes. */
	if (!liscr_script_create_class (self->script, "Class", licomClassScript, self->script) ||
	    !liscr_script_create_class (self->script, "Effect", lisrvEffectScript, self) ||
	    !liscr_script_create_class (self->script, "Extension", lisrvExtensionScript, self) ||
	    !liscr_script_create_class (self->script, "Object", lisrvObjectScript, self) ||
	    !liscr_script_create_class (self->script, "Packet", licomPacketScript, self->script) ||
	    !liscr_script_create_class (self->script, "Path", licomPathScript, self->script) ||
	    !liscr_script_create_class (self->script, "Quaternion", licomQuaternionScript, self->script) ||
	    !liscr_script_create_class (self->script, "Server", lisrvServerScript, self) ||
	    !liscr_script_create_class (self->script, "Vector", licomVectorScript, self->script))
		return 0;
	if (!lisrv_server_init_callbacks_client (self))
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
private_init_sql (lisrvServer* self)
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
private_init_time (lisrvServer* self)
{
	gettimeofday (&self->time.start, NULL);
	return 1;
}

/** @} */
/** @} */

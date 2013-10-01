/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIMai Main
 * @{
 * \addtogroup LIMaiProgram Program
 * @{
 */

#include "main-event.h"
#include "main-extension.h"
#include "main-program.h"

static int private_init (
	LIMaiProgram* self,
	const char*   path,
	const char*   name,
	const char*   args);

static void private_model_added (
	void*       data,
	LIMdlModel* model);

static void private_model_removed (
	void*       data,
	LIMdlModel* model);

static int private_tick (
	LIMaiProgram* self,
	float         secs);

/****************************************************************************/

/**
 * \brief Creates a new program instance.
 *
 * \param path Root data directory.
 * \param name Module name.
 * \param args Module arguments or NULL.
 * \return New program or NULL.
 */
LIMaiProgram* limai_program_new (
	const char* path,
	const char* name,
	const char* args)
{
	LIMaiProgram* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMaiProgram));
	if (self == NULL)
		return NULL;
	self->args = lisys_string_dup ((args != NULL)? args : "");
	if (self->args == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Initialize subsystems. */
	if (!private_init (self, path, name, args))
	{
		limai_program_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the program.
 *
 * \param self Program.
 */
void limai_program_free (
	LIMaiProgram* self)
{
	int i;
	LIMaiExtension* extension;
	LIMaiExtension* extension_next;
	LIMaiEvent* event;
	LIMaiEvent* event_next;
	LIMaiMessage* message;
	LIMaiMessage* message_next;

	/* Invoke callbacks. */
	if (self->callbacks != NULL)
		lical_callbacks_call (self->callbacks, "program-shutdown", lical_marshal_DATA);

	/* Free script. */
	if (self->script != NULL)
		liscr_script_free (self->script);

	/* Free extensions. */
	if (self->extensions != NULL)
	{
		for (extension = self->extensions ; extension != NULL ; extension = extension_next)
		{
			extension_next = extension->next;
			((void (*)(void*)) extension->info->free) (extension->object);
			if (extension->module != NULL)
				lisys_module_free (extension->module);
			lisys_free (extension);
		}
	}

	/* Free components. */
	if (self->components != NULL)
	{
		lisys_assert (self->components->size == 0);
		lialg_strdic_free (self->components);
	}

	/* Free the model manager. */
	if (self->models != NULL)
		limdl_manager_free (self->models);

	/* Invoke callbacks. */
	if (self->callbacks != NULL)
		lical_callbacks_call (self->callbacks, "program-free", lical_marshal_DATA);

	/* Free callbacks. */
	if (self->callbacks != NULL)
	{
		lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
		lical_callbacks_free (self->callbacks);
	}

	if (self->paths != NULL)
		lipth_paths_free (self->paths);

	/* Free messaging. */
	if (self->message_mutex != NULL)
		lisys_mutex_free (self->message_mutex);
	for (i = 0 ; i < LIMAI_MESSAGE_QUEUE_MAX ; i++)
	{
		for (message = self->messages[i] ; message != NULL ; message = message_next)
		{
			message_next = message->next;
			limai_message_free (message);
		}
	}

	/* Free events. */
	for (event = self->events ; event != NULL ; event = event_next)
	{
		event_next = event->next;
		limai_event_free (event);
	}

	lisys_free (self->launch_args);
	lisys_free (self->launch_name);
	lisys_free (self->args);
	lisys_free (self);
}

/**
 * \brief Emits an event.
 * \param self Program.
 * \param type Event type.
 * \param ... List of name,type,value triplets terminated by NULL.
 */
void limai_program_event (
	LIMaiProgram* self,
	const char*   type,
	              ...)
{
	va_list args;

	va_start (args, type);
	limai_program_eventva (self, type, args);
	va_end (args);
}

/**
 * \brief Emits an event.
 * \param self Program.
 * \param type Event type.
 * \param args Variable argument list.
 */
void limai_program_eventva (
	LIMaiProgram* self,
	const char*   type,
	va_list       args)
{
	LIMaiEvent* event;

	event = limai_event_new (type, args);
	if (event == NULL)
		return;
	limai_program_push_event (self, event);
}

/**
 * \brief Executes a script file.
 * \param self Program.
 * \param file Filename.
 * \return Nonzero on success.
 */
int limai_program_execute_script (
	LIMaiProgram* self,
	const char*   file)
{
	int ret;

	/* Load and execute the script. */
	ret = liscr_script_load_file (self->script, file, self->paths->module_data, self->paths->global_data);
	if (!ret)
		return 0;

	return 1;
}

/**
 * \brief Executes a script string.
 * \param self Program.
 * \param code Code string.
 * \return Nonzero on success.
 */
int limai_program_execute_string (
	LIMaiProgram* self,
	const char*   code)
{
	int ret;

	/* Execute the script. */
	ret = liscr_script_load_string (self->script, code, self->paths->module_data, self->paths->global_data);
	if (!ret)
		return 0;

	return 1;
}

/**
 * \brief Finds a component by name.
 *
 * \param self Program.
 * \param name Component name.
 * \return Component or NULL.
 */
void* limai_program_find_component (
	LIMaiProgram* self,
	const char*   name)
{
	return lialg_strdic_find (self->components, name);
}

/**
 * \brief Finds an extension by name.
 *
 * \param self Program.
 * \param name Extension name.
 * \return Extension or NULL.
 */
LIMaiExtension* limai_program_find_extension (
	LIMaiProgram* self,
	const char*   name)
{
	LIMaiExtension* extension;

	for (extension = self->extensions ; extension != NULL ; extension = extension->next)
	{
		if (!strcmp (extension->name, name))
			return extension;
	}

	return NULL;
}

/**
 * \brief Loads an extension.
 * \param self Program.
 * \param name Extensions name.
 * \return Nonzero on success.
 */
int limai_program_insert_extension (
	LIMaiProgram* self,
	const char*   name)
{
	char* ptr;
	char* path;
	char* ident;
	LISysModule* module = NULL;
	LIMaiExtension* extension;
	LIMaiExtensionInfo* info;

	/* Check if already loaded. */
	extension = limai_program_find_extension (self, name);
	if (extension != NULL)
		return 1;

	/* Determine extension info struct name. We restrict the
	 * name of the extension to [a-z_] for security reasons. */
	ident = lisys_string_format ("liext_%s_info", name);
	if (ident == NULL)
		return 0;
	for (ptr = ident ; *ptr != '\0' ; ptr++)
	{
		if (*ptr < 'a' || *ptr > 'z')
			*ptr = '_';
	}

	/* Open an external library if the extension isn't built-in. */
	info = limai_extension_get_builtin (name);
	if (info == NULL)
	{
		if (self->paths->global_exts != NULL)
		{
			lisys_error_set (EINVAL, "cannot open extension `%s'", name);
			lisys_free (ident);
			return 0;
		}
		path = lisys_path_format (self->paths->global_exts, LISYS_PATH_SEPARATOR,
			"lib", name, ".", LISYS_EXTENSION_DLL, NULL);
		if (path == NULL)
		{
			lisys_free (ident);
			return 0;
		}
		module = lisys_module_new (path, 0);
		lisys_free (path);
		if (module == NULL)
		{
			path = lisys_path_format (self->paths->global_exts, LISYS_PATH_SEPARATOR,
				name, ".", LISYS_EXTENSION_DLL, NULL);
			if (path == NULL)
			{
				lisys_free (ident);
				return 0;
			}
			module = lisys_module_new (path, 0);
			lisys_free (path);
			if (module == NULL)
			{
				lisys_error_set (EINVAL, "cannot open extension `%s'", name);
				lisys_free (ident);
				return 0;
			}
		}
		info = lisys_module_symbol (module, ident);
	}
	lisys_free (ident);

	/* Check for valid module info. */
	if (info == NULL)
	{
		lisys_error_set (EINVAL, "cannot load extension `%s': no module info");
		if (module != NULL)
			lisys_module_free (module);
		return 0;
	}
	if (info->version != LIMAI_EXTENSION_VERSION)
	{
		lisys_error_set (EINVAL, "cannot load extension `%s': invalid module version");
		if (module != NULL)
			lisys_module_free (module);
		return 0;
	}
	if (info->name == NULL || info->init == NULL || info->free == NULL)
	{
		lisys_error_set (EINVAL, "cannot load extension `%s': invalid module format");
		if (module != NULL)
			lisys_module_free (module);
		return 0;
	}

	/* Allocate extension. */
	extension = lisys_calloc (1, sizeof (LIMaiExtension));
	if (extension == NULL)
	{
		if (module != NULL)
			lisys_module_free (module);
		return 0;
	}
	strncpy (extension->name, name, sizeof (extension->name) - 1);
	extension->info = info;
	extension->module = module;

	/* Call module initializer. */
	extension->object = ((void* (*)(LIMaiProgram*)) info->init)(self);
	if (extension->object == NULL)
	{
		if (module != NULL)
			lisys_module_free (module);
		lisys_free (extension);
		return 0;
	}

	/* Insert to extension list. */
	/* We prepend to the beginning of the list so that the extensions will be
	   freed in the reverse order. This allows extensions to be cleaned up
	   correctly when they have non-circular dependencies. */
	extension->next = self->extensions;
	self->extensions = extension;

	return 1;
}

/**
 * \brief Registers a component.
 *
 * \param self Program.
 * \param name Component name.
 * \param value Component.
 * \return Nonzero on success.
 */
int limai_program_insert_component (
	LIMaiProgram* self,
	const char*   name,
	void*         value)
{
	return lialg_strdic_insert (self->components, name, value) != NULL;
}

/**
 * \brief Pops an event from the event queue.
 * \param self Program.
 * \return Event, or NULL.
 */
LIMaiEvent* limai_program_pop_event (
	LIMaiProgram* self)
{
	LIMaiEvent* event;

	event = self->events;
	if (event != NULL)
	{
		if (event->next != NULL)
			event->next->prev = NULL;
		self->events = event->next;
	}

	return event;
}

/**
 * \brief Pops a message from the message queue.
 *
 * This function is thread safe. It's specifically intended for thread safe
 * communication between scripted programs running in different threads.
 *
 * \param self Program.
 * \param queue Message queue.
 * \return Message, or NULL.
 */
LIMaiMessage* limai_program_pop_message (
	LIMaiProgram* self,
	int           queue)
{
	LIMaiMessage* message;

	/* Pop a message from the queue. */
	lisys_mutex_lock (self->message_mutex);
	message = self->messages[queue];
	if (message != NULL)
	{
		if (message->next != NULL)
			message->next->prev = NULL;
		self->messages[queue] = message->next;
	}
	lisys_mutex_unlock (self->message_mutex);

	return message;
}

/**
 * \brief Moves events from the event queue to Lua.
 *
 * Events are added to a queue and pushed to scripts separately. This
 * prevents the engine code from triggering garbage collection. That
 * is necessary to avoid arbitrary objects being deleted during the
 * physics update or other critical loops.
 * 
 * \param self Program.
 */
void limai_program_pump_events (
	LIMaiProgram* self)
{
	int count;
	LIMaiEvent* event;
	lua_State* lua;

	/* Get the event queue. */
	lua = liscr_script_get_lua (self->script);
	lua_getglobal (lua, "__events");
	if (lua_type (lua, -1) != LUA_TTABLE)
	{
		lua_pop (lua, 1);
		lua_newtable (lua);
		lua_pushvalue (lua, -1);
		lua_setglobal (lua, "__events");
	}

	/* Get the existing event count. */
#if LUA_VERSION_NUM > 501
	count = lua_rawlen (lua, -1);
#else
	count = lua_objlen (lua, -1);
#endif

	/* Append events to the table. */
	while (1)
	{
		event = limai_program_pop_event (self);
		if (event == NULL)
			break;
		lua_pushnumber (lua, ++count);
		limai_event_write_script (event, self->script);
		limai_event_free (event);
		lua_settable (lua, -3);
	}

	/* Pop the table from the stack. */
	lua_pop (lua, 1);
}

/**
 * \brief Pushes an event to the event queue.
 * \param self Program.
 * \param event Event.
 */
void limai_program_push_event (
	LIMaiProgram* self,
	LIMaiEvent*   event)
{
	LIMaiEvent* ptr;

	if (self->events != NULL)
	{
		for (ptr = self->events ; ptr->next != NULL ; ptr = ptr->next)
			{}
		ptr->next = event;
		event->prev = ptr;
	}
	else
		self->events = event;
}

/**
 * \brief Pushes a message to the message queue.
 *
 * This function is thread safe. It's specifically intended for thread safe
 * communication between scripted programs running in different threads.
 *
 * \param self Program.
 * \param queue Message queue.
 * \param type Message type.
 * \param name Message name.
 * \param data Message data to be soft copied.
 * \return Nonzero on success.
 */
int limai_program_push_message (
	LIMaiProgram* self,
	int           queue,
	int           type,
	const char*   name,
	const void*   data)
{
	LIMaiMessage* ptr;
	LIMaiMessage* message;

	/* Create the message. */
	message = limai_message_new (type, name, data);
	if (message == NULL)
		return 0;

	/* Append it to the message queue. */
	lisys_mutex_lock (self->message_mutex);
	if (self->messages[queue] != NULL)
	{
		for (ptr = self->messages[queue] ; ptr->next != NULL ; ptr = ptr->next)
			{}
		ptr->next = message;
		message->prev = ptr;
	}
	else
		self->messages[queue] = message;
	lisys_mutex_unlock (self->message_mutex);

	return 1;
}

/**
 * \brief Unregisters a component.
 *
 * \param self Program.
 * \param name Component name.
 */
void limai_program_remove_component (
	LIMaiProgram* self,
	const char*   name)
{
	lialg_strdic_remove (self->components, name);
}

/**
 * \brief Tells the server to shut down.
 *
 * \note Thread safe.
 * \param self Program.
 */
void limai_program_shutdown (
	LIMaiProgram* self)
{
	self->quit = 1;
}

/**
 * \brief Executes unit tests for the whole engine.
 * \param self Program.
 */
void limai_program_unittest (
	LIMaiProgram* self)
{
	limat_math_unittest ();
}

/**
 * \brief Updates the program state.
 * \param self Program.
 * \return Nonzero if the program is still running.
 */
int limai_program_update (
	LIMaiProgram* self)
{
	int i;
	float secs;

	/* Calculate time delta. */
	self->curr_tick = lisys_timeval_init ();
	secs = lisys_timeval_get_diff (self->prev_tick, self->curr_tick);
	self->prev_tick = self->curr_tick;

	/* Frames per second. */
	self->ticks[self->ticki++] = secs;
	if (self->ticki == LIMAI_PROGRAM_FPS_TICKS)
		self->ticki = 0;
	self->tick = 0.0f;
	for (i = 0 ; i < LIMAI_PROGRAM_FPS_TICKS ; i++)
		self->tick += self->ticks[i];
	self->fps = LIMAI_PROGRAM_FPS_TICKS / self->tick;
	self->tick = self->tick / LIMAI_PROGRAM_FPS_TICKS;

	/* Update subsystems. */
	lical_callbacks_update (self->callbacks);
	lical_callbacks_call (self->callbacks, "tick", lical_marshal_DATA_FLT, secs);

	/* Sleep until end of frame. */
	if (self->sleep > (int)(1000000 * secs))
		lisys_usleep (self->sleep - (int)(1000000 * secs));

	return !self->quit;
}

/**
 * \brief Gets time since program startup.
 *
 * \param self Program.
 * \return Time in seconds.
 */
double limai_program_get_time (
	const LIMaiProgram* self)
{
	struct timeval t;

	gettimeofday (&t, NULL);
	t.tv_sec -= self->start.tv_sec;
	t.tv_usec -= self->start.tv_usec;
	if (t.tv_usec < 0)
	{
		t.tv_sec -= 1;
		t.tv_usec += 1000000;
	}

	return (double) t.tv_sec + (double) t.tv_usec * 0.000001;
}

/****************************************************************************/

static int private_init (
	LIMaiProgram* self,
	const char*   path,
	const char*   name,
	const char*   args)
{
	/* Initialize paths. */
	self->paths = lipth_paths_new (path, name);
	if (self->paths == NULL)
		return 0;

	/* Initialize dictionaries. */
	self->components = lialg_strdic_new ();
	if (self->components == NULL)
		return 0;

	/* Initialize callbacks. */
	self->callbacks = lical_callbacks_new ();
	if (self->callbacks == NULL)
		return 0;

	/* Allocate the model manager. */
	self->models = limdl_manager_new ();
	if (self->models == NULL)
		return 0;
	self->models->model_added_callback.callback = private_model_added;
	self->models->model_added_callback.userdata = self;
	self->models->model_removed_callback.callback = private_model_removed;
	self->models->model_removed_callback.userdata = self;

	/* Allocate the script. */
	self->script = liscr_script_new ();
	if (self->script == NULL)
		return 0;

	/* Initialize timer. */
	self->start = lisys_timeval_init ();
	self->prev_tick = self->start;
	self->curr_tick = self->start;

	/* Initialize messaging. */
	self->message_mutex = lisys_mutex_new ();
	if (self->message_mutex == NULL)
		return 0;

	/* Register classes. */
	liscr_script_set_userdata (self->script, LISCR_SCRIPT_PROGRAM, self);
	liscr_script_packet (self->script);
	liscr_script_program (self->script);

	/* Register callbacks. */
	if (!lical_callbacks_insert (self->callbacks, "tick", 2, private_tick, self, self->calls))
		return 0;

	lialg_random_init (&self->random, lisys_time (NULL));

	return 1;
}

static void private_model_added (
	void*       data,
	LIMdlModel* model)
{
	LIMaiProgram* self = data;

	/* Invoke callbacks. */
	lical_callbacks_call (self->callbacks, "model-new", lical_marshal_DATA_PTR, model);
}

static void private_model_removed (
	void*       data,
	LIMdlModel* model)
{
	LIMaiProgram* self = data;

	/* Invoke callbacks. */
	lical_callbacks_call (self->callbacks, "model-free", lical_marshal_DATA_PTR, model);
}

static int private_tick (
	LIMaiProgram* self,
	float         secs)
{
	limai_program_event (self, "tick", "secs", LIMAI_FIELD_FLOAT, secs, NULL);

	return 1;
}

/** @} */
/** @} */

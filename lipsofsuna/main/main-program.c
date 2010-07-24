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
 * \addtogroup limai Main
 * @{
 * \addtogroup LIMaiProgram Program
 * @{
 */

#include "main-program.h"

static int private_init (
	LIMaiProgram* self);

static int private_object_model (
	LIMaiProgram* self,
	LIEngObject*  object,
	LIEngModel*   model);

static int private_object_motion (
	LIMaiProgram* self,
	LIEngObject*  object);

static int private_object_new (
	LIMaiProgram* self,
	LIEngObject*  object);

static int private_object_visibility (
	LIMaiProgram* self,
	LIEngObject*  object,
	int           visible);

static int private_tick (
	LIMaiProgram* self,
	float         secs);

/****************************************************************************/

/**
 * \brief Creates a new program instance.
 *
 * \param paths Path information.
 * \return New program or NULL.
 */
LIMaiProgram* limai_program_new (
	LIPthPaths* paths,
	const char* args)
{
	LIMaiProgram* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMaiProgram));
	if (self == NULL)
		return NULL;
	self->paths = paths;
	self->args = listr_dup ((args != NULL)? args : "");
	if (self->args == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Initialize subsystems. */
	if (!private_init (self))
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
	LIAlgStrdicIter iter;
	LIMaiExtension* extension;

	/* Invoke callbacks. */
	if (self->callbacks != NULL)
		lical_callbacks_call (self->callbacks, self, "program-shutdown", lical_marshal_DATA);

	/* Free script. */
	if (self->script != NULL)
		liscr_script_free (self->script);

	/* Free event queue. */
	lialg_list_free (self->event_first);
	self->event_first = NULL;
	self->event_last = NULL;

	/* Free extensions. */
	if (self->extensions != NULL)
	{
		LIALG_STRDIC_FOREACH (iter, self->extensions)
		{
			extension = iter.value;
			((void (*)(void*)) extension->info->free) (extension->object);
			if (extension->module != NULL)
				lisys_module_free (extension->module);
			lisys_free (extension);
		}
		lialg_strdic_free (self->extensions);
	}

	/* Free components. */
	if (self->components != NULL)
	{
		lisys_assert (self->components->size == 0);
		lialg_strdic_free (self->components);
	}

	/* Free engine. */
	if (self->engine != NULL)
		lieng_engine_free (self->engine);

	/* Invoke callbacks. */
	if (self->callbacks != NULL)
		lical_callbacks_call (self->callbacks, self, "program-free", lical_marshal_DATA);

	/* Free callbacks. */
	if (self->callbacks != NULL)
	{
		lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
		lical_callbacks_free (self->callbacks);
	}

	lisys_free (self->launch_args);
	lisys_free (self->launch_name);
	lisys_free (self->args);
	lisys_free (self);
}

/**
 * \brief Emits an event.
 *
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
 *
 * \param self Program.
 * \param type Event type.
 * \param args Variable argument list.
 */
void limai_program_eventva (
	LIMaiProgram* self,
	const char*   type,
	va_list       args)
{
	LIScrData* event;

	/* Create event. */
	event = liscr_event_newv (self->script, args);
	if (event == NULL)
		return;
	liscr_event_set_type (event, type);

	/* Push to event list. */
	limai_program_push_event (self, event);
	liscr_data_unref (event, NULL);
}

/**
 * \brief Executes a script file.
 *
 * \param self Program.
 * \param file Filename.
 * \return Nonzero on success.
 */
int limai_program_execute_script (
	LIMaiProgram* self,
	const char*   file)
{
	int ret;
	char* path;

	/* Load the script. */
	path = lipth_paths_get_script (self->paths, file);
	if (path == NULL)
		return 0;
	ret = liscr_script_load (self->script, path);
	lisys_free (path);
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
	return lialg_strdic_find (self->extensions, name);
}

/**
 * \brief Loads an extension.
 *
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
	LISysModule* module;
	LIMaiExtension* extension;
	LIMaiExtensionInfo* info;

	/* Check if already loaded. */
	module = lialg_strdic_find (self->extensions, name);
	if (module != NULL)
		return 1;

	/* Determine extension info struct name. We restrict the
	 * name of the extension to [a-z_] for security reasons. */
	ident = listr_format ("liext_%s_info", name);
	if (ident == NULL)
		return 0;
	for (ptr = ident ; *ptr != '\0' ; ptr++)
	{
		if (*ptr < 'a' || *ptr > 'z')
			*ptr = '_';
	}

	/* Open an external library if the extension isn't built-in. */
	info = lisys_module_global_symbol (NULL, ident);
	if (info == NULL)
	{
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
				lisys_free (ident);
				goto error;
			}
		}
		info = lisys_module_symbol (module, ident);
	}
	lisys_free (ident);

	/* Check for valid module info. */
	if (info == NULL)
	{
		lisys_error_set (EINVAL, "no module info");
		goto error;
	}
	if (info->version != LIMAI_EXTENSION_VERSION)
	{
		lisys_error_set (EINVAL, "invalid module version");
		goto error;
	}
	if (info->name == NULL || info->init == NULL || info->free == NULL)
	{
		lisys_error_set (EINVAL, "invalid module format");
		goto error;
	}

	/* Insert to extension list. */
	extension = lisys_calloc (1, sizeof (LIMaiExtension));
	if (extension == NULL)
		goto error;
	extension->info = info;
	extension->module = module;
	if (extension == NULL)
		goto error;
	if (!lialg_strdic_insert (self->extensions, name, extension))
	{
		lisys_free (extension);
		goto error;
	}

	/* Call module initializer. */
	extension->object = ((void* (*)(LIMaiProgram*)) info->init)(self);
	if (extension->object == NULL)
	{
		lialg_strdic_remove (self->extensions, name);
		lisys_free (extension);
		goto error;
	}

	return 1;

error:
	lisys_error_append ("cannot initialize module `%s'", name);
	if (module != NULL)
		lisys_module_free (module);
	return 0;
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
 *
 * The returned event has one extra reference that is considered the
 * property of the caller from now on.
 *
 * \param self Program.
 * \return Event or nil.
 */
LIScrData* limai_program_pop_event (
	LIMaiProgram* self)
{
	LIAlgList* ptr;
	LIScrData* event;

	/* Get oldest event. */
	if (self->event_last == NULL)
		return NULL;
	event = self->event_last->data;

	/* Remove from queue. */
	ptr = self->event_last->prev;
	lialg_list_remove (&self->event_first, self->event_last);
	self->event_last = ptr;

	return event;
}

/**
 * \brief Pushes an event to the event queue.
 *
 * The event will be referenced once upon success.
 *
 * \param self Program.
 * \return Nonzero on success.
 */
int limai_program_push_event (
	LIMaiProgram* self,
	LIScrData*    event)
{
	if (!lialg_list_prepend (&self->event_first, event))
		return 0;
	if (self->event_last == NULL)
		self->event_last = self->event_first;
	liscr_data_ref (event, NULL);

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
 * \brief Updates the program state.
 *
 * \param self Program.
 * \return Nonzero if the program is still running.
 */
int limai_program_update (
	LIMaiProgram* self)
{
	int i;
	float secs;

	/* Calculate time delta. */
	gettimeofday (&self->curr_tick, NULL);
	secs = self->curr_tick.tv_sec - self->prev_tick.tv_sec +
	      (self->curr_tick.tv_usec - self->prev_tick.tv_usec) * 0.000001f;
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
	lialg_sectors_update (self->sectors, secs);
	liscr_script_update (self->script, secs);
	lieng_engine_update (self->engine, secs);
	lical_callbacks_call (self->callbacks, self->engine, "tick", lical_marshal_DATA_FLT, secs);

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
	LIMaiProgram* self)
{
	/* Initialize dictionaries. */
	self->components = lialg_strdic_new ();
	if (self->components == NULL)
		return 0;
	self->extensions = lialg_strdic_new ();
	if (self->extensions == NULL)
		return 0;

	/* Initialize callbacks. */
	self->callbacks = lical_callbacks_new ();
	if (self->callbacks == NULL)
		return 0;

	/* Initialize sectors. */
#warning Hardcoded sector size
	self->sectors = lialg_sectors_new (128, 32.0f);
	if (self->sectors == NULL)
		return 0;

	/* Create engine. */
	self->engine = lieng_engine_new (self->callbacks, self->sectors, self->paths->module_data);
	if (self->engine == NULL)
		return 0;
	lieng_engine_set_userdata (self->engine, self);

	/* Allocate the script. */
	self->script = liscr_script_new ();
	if (self->script == NULL)
		return 0;
	liscr_script_set_userdata (self->script, self);

	/* Initialize timer. */
	gettimeofday (&self->start, NULL);
	self->prev_tick = self->start;
	self->curr_tick = self->start;

	/* Register callbacks. */
	if (!lical_callbacks_insert (self->callbacks, self->engine, "object-model", 65535, private_object_model, self, self->calls + 0) ||
	    !lical_callbacks_insert (self->callbacks, self->engine, "object-motion", 63353, private_object_motion, self, self->calls + 1) ||
	    !lical_callbacks_insert (self->callbacks, self->engine, "object-new", 65535, private_object_new, self, self->calls + 2) ||
	    !lical_callbacks_insert (self->callbacks, self->engine, "object-visibility", 65535, private_object_visibility, self, self->calls + 3) ||
	    !lical_callbacks_insert (self->callbacks, self->engine, "tick", 2, private_tick, self, self->calls + 4))
		return 0;

	return 1;
}

static int private_object_model (
	LIMaiProgram* self,
	LIEngObject*  object,
	LIEngModel*   model)
{
	if (object->script != NULL && model != NULL)
	{
		limai_program_event (self, "object-model",
			"object", LISCR_SCRIPT_OBJECT, object->script,
			"model", LISCR_TYPE_STRING, model->name, NULL);
	}

	return 1;
}

static int private_object_motion (
	LIMaiProgram* self,
	LIEngObject*  object)
{
	if (object->script != NULL)
	{
		limai_program_event (self, "object-motion",
			"object", LISCR_SCRIPT_OBJECT, object->script, NULL);
	}

	return 1;
}

static int private_object_new (
	LIMaiProgram* self,
	LIEngObject*  object)
{
	if (object->script != NULL)
	{
		limai_program_event (self, "object-new",
			"object", LISCR_SCRIPT_OBJECT, object->script, NULL);
	}

	return 1;
}

static int private_object_visibility (
	LIMaiProgram* self,
	LIEngObject*  object,
	int           visible)
{
	if (object->script != NULL)
	{
		limai_program_event (self, "object-visibility",
			"object", LISCR_SCRIPT_OBJECT, object->script,
			"visible", LISCR_TYPE_BOOLEAN, visible, NULL);
	}

	return 1;
}

static int private_tick (
	LIMaiProgram* self,
	float        secs)
{
	limai_program_event (self, "tick", "secs", LISCR_TYPE_FLOAT, secs, NULL);

	return 1;
}

/** @} */
/** @} */

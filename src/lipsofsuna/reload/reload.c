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
 * \addtogroup LIRel Reload
 * @{
 * \addtogroup LIRelReload Reload
 * @{
 */

#include <sys/stat.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/string.h>
#include "reload.h"

/**
 * \brief Creates a new data reloader for the engine.
 *
 * The reload object provides automatic data file reloading services. When
 * a DDS or an LMDL file changes, a user provided callback is called so that,
 * for example, the engine can reload the texture or model in question.
 *
 * \param paths Path information.
 * \return New reloader or NULL.
 */
LIRelReload* lirel_reload_new (
	LIPthPaths* paths)
{
	LIRelReload* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRelReload));
	if (self == NULL)
		return NULL;
	self->paths = paths;

	return self;
}

void lirel_reload_free (LIRelReload* self)
{
	lirel_reload_set_enabled (self, 0);
	lisys_free (self);
}

/**
 * \brief Monitors data files for changes.
 *
 * \param self Reload.
 * \return Nonzero on success.
 */
int lirel_reload_update (
	LIRelReload* self)
{
	char* name;
	LISysNotifyEvent* event;

	/* Check for monitor events. */
	if (self->notify == NULL)
		return 1;
	event = lisys_notify_poll (self->notify);
	if (event == NULL)
		return 1;
	if (!(event->flags & LISYS_NOTIFY_CLOSEW))
		return 1;

	/* Reload changed models. */
	if (lisys_path_check_ext (event->name, "lmdl"))
	{
		if (self->reload_model_call != NULL)
		{
			name = lisys_path_format (LISYS_PATH_BASENAME, event->name, LISYS_PATH_STRIPEXTS, NULL);
			if (name != NULL)
			{
				self->reload_model_call (self->reload_model_data, name);
				lisys_free (name);
			}
		}
	}

	/* Reload changed DDS textures. */
	if (lisys_path_check_ext (event->name, "dds"))
	{
		if (self->reload_image_call != NULL)
		{
			name = lisys_path_format (LISYS_PATH_BASENAME,
				event->name, LISYS_PATH_STRIPEXTS, NULL);
			if (name != NULL)
			{
				self->reload_image_call (self->reload_image_data, name);
				lisys_free (name);
			}
		}
	}

	return 1;
}

int lirel_reload_get_enabled (
	const LIRelReload* self)
{
	return (self->notify != NULL);
}

int lirel_reload_set_enabled (
	LIRelReload* self,
	int          value)
{
	char* dstdir;

	if ((value != 0) == (self->notify != NULL))
		return 1;
	if (value)
	{
		self->notify = lisys_notify_new ();
		if (self->notify == NULL)
			return 0;
		dstdir = lisys_path_concat (self->paths->module_data, "graphics", NULL);
		if (dstdir == NULL)
		{
			lisys_notify_free (self->notify);
			self->notify = NULL;
			return 0;
		}
		if (!lisys_notify_add (self->notify, dstdir, LISYS_NOTIFY_CLOSEW))
		{
			lisys_notify_free (self->notify);
			self->notify = NULL;
			lisys_free (dstdir);
		}
		lisys_free (dstdir);
	}
	else
	{
		lisys_notify_free (self->notify);
		self->notify = NULL;
	}

	return 1;
}

void lirel_reload_set_image_callback (
	LIRelReload* self,
	void       (*call)(),
	void*        data)
{
	self->reload_image_call = call;
	self->reload_image_data = data;
}

void lirel_reload_set_model_callback (
	LIRelReload* self,
	void       (*call)(),
	void*        data)
{
	self->reload_model_call = call;
	self->reload_model_data = data;
}

/** @} */
/** @} */
/** @} */

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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliReload Reload
 * @{
 */

#include <sys/stat.h>
#include <client/lips-client.h>
#include <model/lips-model.h>
#include <string/lips-string.h>
#include "ext-reload.h"

static int
private_callback_tick (liextReload* self,
                       float        secs);

static void
private_progress_cancel (liextReload* self);

/*****************************************************************************/

liextReload*
liext_reload_new (licliModule* module)
{
	liextReload* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liextReload));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->module = module;

	/* Allocate reloader. */
	self->reload = lirel_reload_new (module->engine);
	if (self->reload == NULL)
	{
		liext_reload_free (self);
		return NULL;
	}

	/* Allocate callbacks. */
	self->calls[0] = lieng_engine_call_insert (module->engine, LICLI_CALLBACK_TICK, 0, private_callback_tick, self);
	if (self->calls[0] == NULL)
	{
		liext_reload_free (self);
		return NULL;
	}

	return self;
}

void
liext_reload_free (liextReload* self)
{
	if (self->reload != NULL)
	{
		liext_reload_cancel (self);
		lirel_reload_free (self->reload);
	}
	free (self);
}

/**
 * \brief Cancels any active reloading process.
 *
 * \param self Reload.
 */
void
liext_reload_cancel (liextReload* self)
{
	lirel_reload_cancel (self->reload);
	private_callback_tick (self, 1.0f);
}

/**
 * \brief Reloads all client data files.
 *
 * \param self Reload.
 * \return Nonzero on success.
 */
int
liext_reload_run (liextReload* self)
{
	return lirel_reload_run (self->reload);
}

int
liext_reload_get_enabled (const liextReload* self)
{
	return lirel_reload_get_enabled (self->reload);
}

int
liext_reload_set_enabled (liextReload* self,
                          int          value)
{
	return lirel_reload_set_enabled (self->reload, value);
}

/*****************************************************************************/

/**
 * \brief Monitors data files for changes if enabled.
 *
 * \param self Reload.
 */
int
private_callback_tick (liextReload* self,
                       float        secs)
{
	/* Update reloader state. */
	lirel_reload_update (self->reload);

	/* Delete progress dialog on demand. */
	if (lirel_reload_get_done (self->reload))
	{
		if (self->progress != NULL)
		{
			liwdg_manager_remove_window (self->module->widgets, LIWDG_WIDGET (self->progress));
			liwdg_widget_free (self->progress);
			self->progress = NULL;
		}
		return 1;
	}

	/* Create progress dialog on demand. */
	if (self->progress == NULL)
	{
		self->progress = liwdg_busy_new (self->module->widgets);
		if (self->progress == NULL)
		{
			lirel_reload_cancel (self->reload);
			return 1;
		}
		liwdg_busy_set_cancel (LIWDG_BUSY (self->progress), LIWDG_HANDLER (private_progress_cancel), self);
		liwdg_busy_set_text (LIWDG_BUSY (self->progress), "Loading...");
		liwdg_widget_set_visible (LIWDG_WIDGET (self->progress), 1);
		liwdg_manager_insert_window (self->module->widgets, LIWDG_WIDGET (self->progress));
	}

	/* Update progress. */
	liwdg_busy_set_progress (LIWDG_BUSY (self->progress),
		lirel_reload_get_progress (self->reload));

	return 1;
}

/**
 * \brief Called when the cancel button is pressed in the progress dialog.
 *
 * Sends a signal to the worker thread to stop working.
 *
 * \param self Reload.
 */
static void
private_progress_cancel (liextReload* self)
{
	lirel_reload_cancel (self->reload);
}

/** @} */
/** @} */
/** @} */

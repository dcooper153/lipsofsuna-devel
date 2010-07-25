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
 * \addtogroup liext Extension
 * @{
 * \addtogroup LIExtPackager Packager
 * @{
 */

#include "ext-module.h"
#include "ext-packager.h"
#include "ext-resources.h"

#define ENABLE_PACKAGE_SERVER

static void
private_async_free (LIThrAsyncCall* call,
                    void*           data);

static void
private_async_save (LIThrAsyncCall* call,
                    void*           data);

static int
private_ignore (LIExtPackagerData* self,
                const char*        name);

static void
private_progress_cancel (LIExtPackager* self);

static void
private_progress_update (LIExtPackager* self);

/*****************************************************************************/

LIExtPackager*
liext_packager_new (LIMaiProgram* program)
{
	LIExtPackager* self;

	self = lisys_calloc (1, sizeof (LIExtPackager));
	if (self == NULL)
		return NULL;
	self->program = program;
	self->client = limai_program_find_component (program, "client");

	/* Register classes. */
	liscr_script_create_class (program->script, "Packager", liext_script_packager, self);

	return self;
}

void
liext_packager_free (LIExtPackager* self)
{
	liext_packager_cancel (self);
	lisys_free (self);
}

/**
 * \brief Cancels any active packaging process.
 *
 * \param self Packager.
 */
void
liext_packager_cancel (LIExtPackager* self)
{
	if (self->worker == NULL)
		return;
	lithr_async_call_stop (self->worker);
	lithr_async_call_join (self->worker);
	private_progress_update (self);
}

/**
 * \brief Saves all client data files to a data tarball.
 *
 * The saved files include models, textures, shaders, sounds, fonts,
 * config, and client scripts.
 *
 * \param self Packager.
 * \param name Target file name.
 * \param dir Root directory inside the archive.
 * \return Nonzero on success.
 */
int
liext_packager_save (LIExtPackager* self,
                     const char*    name,
                     const char*    dir)
{
	LIExtPackagerData* data;

	/* Make sure not already running. */
	if (self->worker != NULL)
	{
		lisys_error_set (EBUSY, "busy saving");
		return 0;
	}

	/* Allocate data. */
	data = lisys_calloc (1, sizeof (LIExtPackagerData));
	if (data == NULL)
		return 0;
	data->packager = self;
	data->client = self->client;
	data->target = listr_dup (name);
	if (data->target == NULL)
	{
		lisys_free (data);
		return 0;
	}
	data->directory = listr_dup (dir);
	if (data->directory == NULL)
	{
		lisys_free (data->target);
		lisys_free (data);
		return 0;
	}

	/* Create worker thread. */
	self->worker = lithr_async_call_new (private_async_save, private_async_free, data);
	if (self->worker == NULL)
	{
		lisys_free (data->target);
		lisys_free (data->directory);
		lisys_free (data);
		return 0;
	}

	/* Create progress dialog. */
	self->progress = liwdg_busy_new (self->client->widgets);
	if (self->progress == NULL)
	{
		lithr_async_call_stop (self->worker);
		lithr_async_call_join (self->worker);
		lithr_async_call_free (self->worker);
		self->worker = NULL;
		return 0;
	}
	liwdg_busy_set_cancel (LIWDG_BUSY (self->progress), LIWDG_HANDLER (private_progress_cancel), self);
	liwdg_busy_set_update (LIWDG_BUSY (self->progress), LIWDG_HANDLER (private_progress_update), self);
	liwdg_busy_set_text (LIWDG_BUSY (self->progress), "Packaging...");
	liwdg_widget_set_visible (LIWDG_WIDGET (self->progress), 1);
	liwdg_manager_insert_window (self->client->widgets, LIWDG_WIDGET (self->progress));

	return 1;
}

/**
 * \brief Checks if verbose debug messages are enabled.
 *
 * \param self Packager.
 * \return Nonzero if verbose debug messages are enabled.
 */
int
liext_packager_get_verbose (LIExtPackager* self)
{
	return self->verbose;
}

/**
 * \brief Enables or disables verbose debug messages.
 *
 * \param self Packager.
 * \param value Boolean.
 */
void
liext_packager_set_verbose (LIExtPackager* self,
                            int            value)
{
	self->verbose = value;
}

/*****************************************************************************/

static void
private_async_free (LIThrAsyncCall* call,
                    void*           data)
{
	LIExtPackagerData* self = data;

	if (self->resources != NULL)
		liext_resources_free (self->resources);
	if (self->tar != NULL)
		liarc_tar_free (self->tar);
	if (self->writer != NULL)
		liarc_writer_free (self->writer);
	lisys_free (self->target);
	lisys_free (self->directory);
}

static void
private_async_save (LIThrAsyncCall* call,
                    void*           data)
{
	int i;
	int j;
	int total;
	const char* src;
	const char* dst;
	LIExtPackagerData* self = data;

	/* Allocate writer. */
	self->writer = liarc_writer_new_gzip (self->target);
	if (self->writer == NULL)
		goto error;
	self->tar = liarc_tar_new (self->writer);
	if (self->tar == NULL)
		goto error;

	/* Create resource list. */
	self->resources = liext_resources_new ();
	if (self->resources == NULL)
		goto error;

	/* Setup ignores. */
	if (!private_ignore (self, "import") ||
	    !private_ignore (self, "concept") ||
	    !private_ignore (self, "Assets.xml") ||
	    !private_ignore (self, "Damfile"))
		goto error;

	/* Collect files and directories. */
	if (!liext_resources_insert_directory (self->resources, self->client->path, self->directory))
		goto error;
	if (lithr_async_call_get_stop (call))
		goto stop;
	total = self->resources->directories.count + self->resources->files.count;
	if (self->packager->verbose)
	{
		printf ("Packager: found %d directories\n", self->resources->directories.count);
		printf ("Packager: found %d files\n", self->resources->files.count);
	}

	/* Write directories. */
	for (i = j = 0 ; i < self->resources->directories.count ; i++, j++)
	{
		dst = self->resources->directories.array[i];
		lithr_async_call_set_progress (call, (float) j / total);
		if (self->packager->verbose)
			printf ("Packager: mkdir %s\n", dst);
		if (!liarc_tar_write_directory (self->tar, dst))
			goto error;
		if (lithr_async_call_get_stop (call))
			goto stop;
	}

	/* Write files. */
	for (i = 0 ; i < self->resources->files.count ; i++, j++)
	{
		src = self->resources->files.array[i]->src;
		dst = self->resources->files.array[i]->dst;
		lithr_async_call_set_progress (call, (float) j / total);
		if (self->packager->verbose)
			printf ("Packager: cp %s %s\n", src, dst);
		if (!liarc_tar_write_file (self->tar, src, dst))
			goto error;
		if (lithr_async_call_get_stop (call))
			goto stop;
	}

	/* Write to disk. */
	lithr_async_call_set_progress (call, 1.0f);
	if (lithr_async_call_get_stop (call))
		goto stop;
	if (!liarc_tar_write_end (self->tar))
		goto error;
	lithr_async_call_set_result (call, 1);

stop:
	return;

error:
	lisys_error_report ();
}

static int
private_ignore (LIExtPackagerData* self,
                const char*        name)
{
	char* tmp;

	tmp = lisys_path_concat (self->directory, name, NULL);
	if (tmp == NULL)
		return 0;
	if (!liext_resources_insert_ignore (self->resources, tmp))
	{
		lisys_free (tmp);
		return 0;
	}
	lisys_free (tmp);

	return 1;
}

/**
 * \brief Called when the cancel button is pressed in the progress dialog.
 *
 * Sends a signal to the worker thread to stop working.
 *
 * \param self Module.
 */
static void
private_progress_cancel (LIExtPackager* self)
{
	lithr_async_call_stop (self->worker);
}

/**
 * \brief Called every tick to update packager status.
 *
 * If packaging is still in process, updates the progress dialog status.
 * If packaging has ended, frees the progress dialog and the worker thread.
 *
 * \param self Module.
 */
static void
private_progress_update (LIExtPackager* self)
{
	if (self->worker == NULL)
		return;
	if (lithr_async_call_get_done (self->worker))
	{
		liwdg_manager_remove_window (self->client->widgets, self->progress);
		lithr_async_call_free (self->worker);
		self->progress = NULL;
		self->worker = NULL;
	}
	else
		liwdg_busy_set_progress (LIWDG_BUSY (self->progress), lithr_async_call_get_progress (self->worker));
}

/** @} */
/** @} */

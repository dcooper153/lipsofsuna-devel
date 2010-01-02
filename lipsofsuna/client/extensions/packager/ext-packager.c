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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliPackager Packager
 * @{
 */

#include <archive/lips-archive.h>
#include <client/lips-client.h>
#include <model/lips-model.h>
#include <render/lips-render.h>
#include <string/lips-string.h>
#include <system/lips-system.h>
#include <thread/lips-thread.h>
#include <widget/lips-widget.h>
#include "ext-packager.h"
#include "ext-resources.h"

#define ENABLE_PACKAGE_SERVER

static void
private_async_free (lithrAsyncCall* call,
                    void*           data);

static void
private_async_save (lithrAsyncCall* call,
                    void*           data);

static inline int
private_filter_models (const char* dir,
                       const char* name,
                       void*       data);

static inline int
private_filter_samples (const char* dir,
                        const char* name,
                        void*       data);

static int
private_insert_directory (liextPackagerData* self,
                          const char*        path,
                          const char*        name);

static int
private_insert_extra (lithrAsyncCall*    call,
                      liextPackagerData* self);

static int
private_insert_file (liextPackagerData* self,
                     const char*        dir,
                     const char*        name,
                     const char*        ext);

static inline int
private_insert_models (lithrAsyncCall*    call,
                       liextPackagerData* self,
                       const char*        path);

static inline int
private_insert_samples (lithrAsyncCall*    call,
                        liextPackagerData* self,
                        const char*        path);

static void
private_progress_cancel (liextPackager* self);

static void
private_progress_update (liextPackager* self);

static void
private_verbose_model (liextPackagerData* self,
                       limdlModel*        model,
                       const char*        name);

static int
private_write_directory (liextPackagerData* self,
                         const char*        name);

/*****************************************************************************/

liextPackager*
liext_packager_new (licliClient* client)
{
	liextPackager* self;

	self = lisys_calloc (1, sizeof (liextPackager));
	if (self == NULL)
		return NULL;
	self->client = client;

	return self;
}

void
liext_packager_free (liextPackager* self)
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
liext_packager_cancel (liextPackager* self)
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
liext_packager_save (liextPackager* self,
                     const char*    name,
                     const char*    dir)
{
	liextPackagerData* data;

	/* Make sure not already running. */
	if (self->worker != NULL)
	{
		lisys_error_set (EBUSY, "busy saving");
		return 0;
	}

	/* Allocate data. */
	data = lisys_calloc (1, sizeof (liextPackagerData));
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
liext_packager_get_verbose (liextPackager* self)
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
liext_packager_set_verbose (liextPackager* self,
                            int            value)
{
	self->verbose = value;
}

/*****************************************************************************/

static void
private_async_free (lithrAsyncCall* call,
                    void*           data)
{
	int i;
	liextPackagerData* self = data;

	if (self->resources != NULL)
		liext_resources_free (self->resources);
	if (self->tar != NULL)
		liarc_tar_free (self->tar);
	if (self->writer != NULL)
		liarc_writer_free (self->writer);
	for (i = 0 ; i < self->files.count ; i++)
	{
		lisys_free (self->files.array[i].src);
		lisys_free (self->files.array[i].dst);
	}
	lisys_free (self->files.array);
	lisys_free (self->target);
	lisys_free (self->directory);
}

static void
private_async_save (lithrAsyncCall* call,
                    void*           data)
{
	int i;
	char* path;
	const char* name;
	liextPackagerData* self = data;

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
	path = lisys_path_concat (self->client->path, "graphics", NULL);
	if (path == NULL)
		goto error;
	if (!private_insert_models (call, self, path) ||
	    !private_insert_extra (call, self))
	{
		lisys_free (path);
		goto error;
	}
	lisys_free (path);
	if (lithr_async_call_get_stop (call))
		goto stop;

	/* Collect graphics. */
	for (i = 0 ; i < self->resources->models.count ; i++)
	{
		name = self->resources->models.array[i].name;
		if (!private_insert_file (self, "graphics", name, ".lmdl"))
			goto error;
		if (lithr_async_call_get_stop (call))
			goto stop;
	}
#if 0
	for (i = 0 ; i < self->resources->shaders.count ; i++)
	{
		name = self->resources->shaders.array[i];
		if (!private_insert_file (self, "shaders", name, ""))
			goto error;
		if (lithr_async_call_get_stop (call))
			goto stop;
	}
#endif
	for (i = 0 ; i < self->resources->textures.count ; i++)
	{
		name = self->resources->textures.array[i];
		if (!private_insert_file (self, "graphics", name, ".dds"))
			goto error;
		if (lithr_async_call_get_stop (call))
			goto stop;
	}

	/* Collect sound effects. */
	path = lisys_path_concat (self->client->path, "sounds", NULL);
	if (path == NULL)
		goto error;
	if (!private_insert_samples (call, self, path))
	{
		lisys_free (path);
		goto error;
	}
	lisys_free (path);
	if (lithr_async_call_get_stop (call))
		goto stop;

	/* Collect miscellaneous. */
	if (!private_insert_directory (self, "", "about") ||
	    !private_insert_directory (self, "", "config") ||
	    !private_insert_directory (self, "", "fonts") ||
	    !private_insert_directory (self, "", "shaders") ||
	    !private_insert_directory (self, "scripts", "client") ||
#ifdef ENABLE_PACKAGE_SERVER
	    !private_insert_directory (self, "scripts", "server") ||
	    !private_insert_directory (self, "", "save")
#endif
       )
		goto error;
	if (lithr_async_call_get_stop (call))
		goto stop;

	/* Create archive. */
	if (!liarc_tar_write_directory (self->tar, self->directory) ||
	    !private_write_directory (self, "data/") ||
	    !private_write_directory (self, "data/about/") ||
	    !private_write_directory (self, "data/config/") ||
	    !private_write_directory (self, "data/graphics/") ||
	    !private_write_directory (self, "data/scripts/") ||
	    !private_write_directory (self, "data/scripts/client/") ||
	    !private_write_directory (self, "data/scripts/server/") ||
	    !private_write_directory (self, "data/shaders/") ||
	    !private_write_directory (self, "data/sounds/") ||
#ifdef ENABLE_PACKAGE_SERVER
	    !private_write_directory (self, "data/save/")
#endif
	   )
		goto error;
	for (i = 0 ; i < self->files.count ; i++)
	{
		lithr_async_call_set_progress (call, (float) i / self->files.count);
		if (lithr_async_call_get_stop (call))
			goto stop;
		if (!liarc_tar_write_file (self->tar, self->files.array[i].src, self->files.array[i].dst))
			goto error;
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

static inline int
private_filter_models (const char* dir,
                       const char* name,
                       void*       data)
{
	const char* ptr;

	ptr = strstr (name, ".lmdl");
	if (ptr == NULL)
		return 0;
	if (strcmp (ptr, ".lmdl"))
		return 0;
	return 1;
}

static inline int
private_filter_samples (const char* dir,
                        const char* name,
                        void*       data)
{
	const char* ptr;

	ptr = strstr (name, ".ogg");
	if (ptr == NULL)
		return 0;
	if (strcmp (ptr, ".ogg"))
		return 0;
	return 1;
}

static int
private_insert_directory (liextPackagerData* self,
                          const char*        path,
                          const char*        name)
{
	int i;
	int ret;
	char* src;
	const char* file;
	liextPackagerFile tmp;
	lisysDir* dir;

	src = lisys_path_concat (self->client->path, path, name, NULL);
	if (src == NULL)
		return 0;
	dir = lisys_dir_open (src);
	lisys_free (src);
	if (dir == NULL)
		return 0;
	lisys_dir_set_filter (dir, LISYS_DIR_FILTER_VISIBLE, NULL);
	if (!lisys_dir_scan (dir))
		return 0;
	for (i = 0 ; i < lisys_dir_get_count (dir) ; i++)
	{
		file = lisys_dir_get_name (dir, i);
		tmp.src = lisys_dir_get_path (dir, i);
		tmp.dst = lisys_path_concat (self->directory, "data", path, name, file, NULL);
		if (tmp.src != NULL && tmp.dst != NULL)
			ret = lialg_array_append (&self->files, &tmp);
		else
			ret = 0;
		if (!ret)
		{
			lisys_free (tmp.src);
			lisys_free (tmp.dst);
			return 0;
		}
	}

	return 1;
}

static int
private_insert_extra (lithrAsyncCall*    call,
                      liextPackagerData* self)
{
	char* tmp;
	char* path;
	liarcReader* reader;

	/* Open extra texture list. */
	path = lisys_path_concat (self->client->path, "graphics", "textures.cfg", NULL);
	if (path == NULL)
		return 0;
	reader = liarc_reader_new_from_file (path);
	lisys_free (path);
	if (reader == NULL)
	{
		if (lisys_error_peek () != EIO)
			return 0;
		lisys_error_get (NULL);
		return 1;
	}

	/* Read texture lines. */
	while (!liarc_reader_check_end (reader))
	{
		if (!liarc_reader_get_text (reader, "\n", &tmp))
		{
			liarc_reader_free (reader);
			return 0;
		}
		if (!liext_resources_insert_texture (self->resources, tmp))
		{
			lisys_free (tmp);
			return 0;
		}
		lisys_free (tmp);
	}
	liarc_reader_free (reader);

	return 1;
}

static int
private_insert_file (liextPackagerData* self,
                     const char*        dir,
                     const char*        name,
                     const char*        ext)
{
	int ret;
	liextPackagerFile tmp;

	tmp.src = lisys_path_format (self->client->path,
		LISYS_PATH_SEPARATOR, dir,
		LISYS_PATH_SEPARATOR, name, ext, NULL);
	tmp.dst = lisys_path_format (self->directory,
		LISYS_PATH_SEPARATOR, "data",
		LISYS_PATH_SEPARATOR, dir,
		LISYS_PATH_SEPARATOR, name, ext, NULL);
	if (tmp.src != NULL && tmp.dst != NULL)
		ret = lialg_array_append (&self->files, &tmp);
	else
		ret = 0;
	if (!ret)
	{
		lisys_free (tmp.src);
		lisys_free (tmp.dst);
	}

	return ret;
}

static inline int
private_insert_models (lithrAsyncCall*    call,
                       liextPackagerData* self,
                       const char*        path)
{
	int i;
	int ret;
	int count;
	char* file;
	char* name;
	limdlModel* model = NULL;
	lisysDir* directory = NULL;

	/* Find all converted models. */
	directory = lisys_dir_open (path);
	if (directory == NULL)
		return 0;
	lisys_dir_set_filter (directory, private_filter_models, NULL);
	lisys_dir_set_sorter (directory, LISYS_DIR_SORTER_ALPHA);
	if (!lisys_dir_scan (directory))
		goto error;
	count = lisys_dir_get_count (directory);

	/* Load model and node information. */
	/* FIXME: Use cache unless some models were rebuilt above. */
	for (i = 0 ; i < count ; i++)
	{
		lithr_async_call_set_progress (call, (float) i / count);
		if (lithr_async_call_get_stop (call))
			break;

		/* Open model file. */
		file = lisys_dir_get_path (directory, i);
		if (file == NULL)
			goto error;
		model = limdl_model_new_from_file (file);
		lisys_free (file);
		if (model == NULL)
			goto error;

		/* Verbose messages. */
		if (self->packager->verbose)
			private_verbose_model (self, model, lisys_dir_get_name (directory, i));

		/* Add to resource list. */
		name = lisys_path_format (lisys_dir_get_name (directory, i), LISYS_PATH_STRIPEXT, NULL);
		if (name == NULL)
			goto error;
		ret = liext_resources_insert_model (self->resources, name, model);
		lisys_free (name);
		if (!ret)
			goto error;

		/* Free the model. */
		limdl_model_free (model);
	}
	lisys_dir_free (directory);

	return 1;

error:
	if (model != NULL)
		limdl_model_free (model);
	lisys_dir_free (directory);
	return 0;
}

static int
private_insert_samples (lithrAsyncCall*    call,
                        liextPackagerData* self,
                        const char*        path)
{
	int i;
	int count;
	const char* name;
	lisysDir* directory = NULL;

	/* Find all sound and music files. */
	directory = lisys_dir_open (path);
	if (directory == NULL)
		return 0;
	lisys_dir_set_filter (directory, private_filter_samples, NULL);
	lisys_dir_set_sorter (directory, LISYS_DIR_SORTER_ALPHA);
	if (!lisys_dir_scan (directory))
		goto error;
	count = lisys_dir_get_count (directory);

	/* Insert found files. */
	for (i = 0 ; i < count ; i++)
	{
		lithr_async_call_set_progress (call, (float) i / count);
		if (lithr_async_call_get_stop (call))
			break;
		name = lisys_dir_get_name (directory, i);
		if (!private_insert_file (self, "sounds", name, ""))
			goto error;
	}
	lisys_dir_free (directory);

	return 1;

error:
	lisys_dir_free (directory);
	return 0;
}

/**
 * \brief Called when the cancel button is pressed in the progress dialog.
 *
 * Sends a signal to the worker thread to stop working.
 *
 * \param self Module.
 */
static void
private_progress_cancel (liextPackager* self)
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
private_progress_update (liextPackager* self)
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

static void
private_verbose_model (liextPackagerData* self,
                       limdlModel*        model,
                       const char*        name)
{
	int i;
	int j;
	limdlMaterial* material;

	printf ("Model: %s\n", name);
	for (i = 0 ; i < model->materials.count ; i++)
	{
		material = model->materials.array + i;
		for (j = 0 ; j < material->textures.count ; j++)
			printf ("    %s\n", material->textures.array[j].string);
	}
}

static int
private_write_directory (liextPackagerData* self,
                         const char*        name)
{
	char* path;

	path = lisys_path_concat (self->directory, name, NULL);
	if (path == NULL)
		return 0;
	if (!liarc_tar_write_directory (self->tar, path))
	{
		lisys_free (path);
		return 0;
	}

	return 1;
}

/** @} */
/** @} */
/** @} */

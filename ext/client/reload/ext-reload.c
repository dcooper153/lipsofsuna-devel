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

static void
private_async_reload (lithrAsyncCall* call);

static int
private_callback_tick (liextReload* self,
                       float        secs);

static int
private_convert_models (lithrAsyncCall* call,
                        liextReload*    self,
                        const char*     path);

static int
private_convert_textures (lithrAsyncCall* call,
                          liextReload*    self,
                          const char*     path);

static int
private_filter_blend_modified (const char* dir,
                               const char* name);

static int
private_filter_img_modified (const char* dir,
                             const char* name);

static int
private_filter_xcf_modified (const char* dir,
                             const char* name);

static void
private_progress_cancel (liextReload* self);

static void
private_progress_update (liextReload* self);

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

	/* Allocate callbacks. */
	self->calls[0] = lieng_engine_call_insert (module->engine, LICLI_CALLBACK_TICK, 0, private_callback_tick, self);
	if (self->calls[0] == NULL)
	{
		free (self);
		return NULL;
	}

	return self;
}

void
liext_reload_free (liextReload* self)
{
	liext_reload_cancel (self);
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
	if (self->worker == NULL)
		return;
	lithr_async_call_stop (self->worker);
	lithr_async_call_wait (self->worker);
	private_progress_update (self);
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
	/* Make sure not already running. */
	if (self->worker != NULL)
	{
		lisys_error_set (EBUSY, "busy reloading");
		return 0;
	}

	/* Create worker thread. */
	self->worker = lithr_async_call_new (private_async_reload, NULL, self);
	if (self->worker == NULL)
		return 0;

	/* Create progress dialog. */
	self->progress = liwdg_busy_new (self->module->widgets);
	if (self->progress == NULL)
	{
		lithr_async_call_stop (self->worker);
		lithr_async_call_wait (self->worker);
		lithr_async_call_free (self->worker);
		self->worker = NULL;
		return 0;
	}
	liwdg_busy_set_cancel (LIWDG_BUSY (self->progress), LIWDG_HANDLER (private_progress_cancel), self);
	liwdg_busy_set_update (LIWDG_BUSY (self->progress), LIWDG_HANDLER (private_progress_update), self);
	liwdg_busy_set_text (LIWDG_BUSY (self->progress), "Loading...");
	liwdg_widget_set_visible (LIWDG_WIDGET (self->progress), 1);
	liwdg_manager_insert_window (self->module->widgets, LIWDG_WIDGET (self->progress));

	return 1;
}

int
liext_reload_get_enabled (const liextReload* self)
{
	return (self->notify != NULL);
}

int
liext_reload_set_enabled (liextReload* self,
                          int          value)
{
	char* path;

	if ((value != 0) == (self->notify != NULL))
		return 1;
	if (value)
	{
		self->notify = lisys_notify_new ();
		if (self->notify == NULL)
			return 0;
		path = lisys_path_concat (self->module->path, "graphics", NULL);
		if (path == NULL)
		{
			lisys_notify_free (self->notify);
			self->notify = NULL;
			return 0;
		}
		if (!lisys_notify_add (self->notify, path, LISYS_NOTIFY_CLOSEW))
		{
			lisys_notify_free (self->notify);
			self->notify = NULL;
			free (path);
		}
		free (path);
	}
	else
	{
		lisys_notify_free (self->notify);
		self->notify = NULL;
	}

	return 1;
}

/*****************************************************************************/

static void
private_async_reload (lithrAsyncCall* call)
{
	char* path;
	liextReload* self = call->data;

	/* Convert textures. */
	path = lisys_path_concat (self->module->path, "graphics", NULL);
	if (path == NULL)
		goto error;
	if (!private_convert_textures (call, self, path))
		goto error;
	if (call->stop)
		goto stop;

	/* Convert models. */
	if (path == NULL)
		goto error;
	if (!private_convert_models (call, self, path))
		goto error;
	if (call->stop)
		goto stop;

	/* The rest is done in the free callback. */
	call->result = 1;
stop:
	free (path);
	return;

error:
	free (path);
}

/**
 * \brief Monitors data files for changes if enabled.
 *
 * \param self Reload.
 */
int
private_callback_tick (liextReload* self,
                       float        secs)
{
	char* name;

	if (self->notify == NULL)
		return 1;
	if (!lisys_notify_poll (self->notify))
		return 1;
	if (!(self->notify->event.flags & LISYS_NOTIFY_CLOSEW))
		return 1;

	/* Reload changed models. */
	if (lisys_path_check_ext (self->notify->event.name, "lmdl"))
	{
		name = lisys_path_format (LISYS_PATH_BASENAME,
			self->notify->event.name, LISYS_PATH_STRIPEXTS, NULL);
		printf ("Reloading model `%s'\n", name);
		if (name != NULL)
		{
			lieng_engine_load_model (self->module->engine, name);
			free (name);
		}
	}

	/* Reload changed DDS textures. */
	if (lisys_path_check_ext (self->notify->event.name, "dds"))
	{
		name = lisys_path_format (LISYS_PATH_BASENAME,
			self->notify->event.name, LISYS_PATH_STRIPEXTS, NULL);
		printf ("Reloading texture `%s'\n", name);
		if (name != NULL)
		{
			lieng_engine_load_texture (self->module->engine, name);
			free (name);
		}
	}

	/* Initiate conversion if an original has changed. */
	if (lisys_path_check_ext (self->notify->event.name, "blend") ||
	    lisys_path_check_ext (self->notify->event.name, "blend.gz") ||
	    lisys_path_check_ext (self->notify->event.name, "jpg") ||
	    lisys_path_check_ext (self->notify->event.name, "png") ||
	    lisys_path_check_ext (self->notify->event.name, "xcf") ||
	    lisys_path_check_ext (self->notify->event.name, "xcf.bz2") ||
	    lisys_path_check_ext (self->notify->event.name, "xcf.gz") ||
	    lisys_path_check_ext (self->notify->event.name, "xcfbz2") ||
	    lisys_path_check_ext (self->notify->event.name, "xcfgz"))
	{
		printf ("Reloading originals...\n");
		self->queued = 1;
	}
	if (self->queued && self->worker == NULL)
	{
		if (liext_reload_run (self))
			self->queued = 0;
	}

	return 1;
}

static int
private_convert_models (lithrAsyncCall* call,
                        liextReload*    self,
                        const char*     path)
{
	int i;
	int count;
	char* src = NULL;
	char* dst = NULL;
	lisysDir* directory = NULL;

	/* Find all modified model sources. */
	directory = lisys_dir_open (path);
	if (directory == NULL)
		return 0;
	lisys_dir_set_filter (directory, private_filter_blend_modified);
	lisys_dir_set_sorter (directory, LISYS_DIR_SORTER_ALPHA);
	if (!lisys_dir_scan (directory))
		goto error;
	count = lisys_dir_get_count (directory);

	/* Convert modified models. */
	for (i = 0 ; i < count ; i++)
	{
		call->progress = (float) i / count;
		if (call->stop)
			break;
		src = lisys_dir_get_path (directory, i);
		dst = lisys_path_format (src, LISYS_PATH_STRIPEXTS, ".lmdl", NULL);
		if (src == NULL || dst == NULL)
			goto error;
		if (!liext_reload_blender (self, src, dst))
			lisys_error_report ();
		free (src);
		free (dst);
	}
	lisys_dir_free (directory);

	return 1;

error:
	lisys_dir_free (directory);
	free (src);
	free (dst);
	return 0;
}

static int
private_convert_textures (lithrAsyncCall* call,
                          liextReload*    self,
                          const char*     path)
{
	int i;
	int j;
	int count;
	char* src;
	char* dst;
	lisysDir* directory = NULL;
	const struct
	{
		int (*filter)(const char*, const char*);
		int (*convert)(const char*, const char*);
	}
	converters[] =
	{
		{ private_filter_xcf_modified, liext_reload_gimp },
		{ private_filter_img_modified, liext_reload_image }
	};

	/* Convert all modified texture sources. */
	for (j = 0 ; j < sizeof (converters) / sizeof (*converters) ; j++)
	{
		src = NULL;
		dst = NULL;
		directory = lisys_dir_open (path);
		if (directory == NULL)
			return 0;
		lisys_dir_set_filter (directory, converters[j].filter);
		lisys_dir_set_sorter (directory, LISYS_DIR_SORTER_ALPHA);
		if (!lisys_dir_scan (directory))
			goto error;
		count = lisys_dir_get_count (directory);
		for (i = 0 ; i < count ; i++)
		{
			call->progress = (float) i / count;
			if (call->stop)
				break;
			src = lisys_dir_get_path (directory, i);
			dst = lisys_path_format (src, LISYS_PATH_STRIPEXTS, ".dds", NULL);
			if (src == NULL || dst == NULL)
				goto error;
			if (!converters[j].convert (src, dst))
				lisys_error_report ();
			free (src);
			free (dst);
		}
		lisys_dir_free (directory);
	}

	return 1;

error:
	lisys_dir_free (directory);
	free (src);
	free (dst);
	return 0;
}

static int
private_filter_blend_modified (const char* dir,
                               const char* name)
{
	int ret;
	char* src;
	char* dst;
	struct stat src_st;
	struct stat dst_st;

	/* Check for extension. */
	if (!lisys_path_check_ext (name, "blend") &&
	    !lisys_path_check_ext (name, "blend.gz"))
		return 0;

	/* Construct file names. */
	src = lisys_path_concat (dir, name, NULL);
	if (src == NULL)
		return 1;
	dst = lisys_path_format (src, LISYS_PATH_STRIPEXTS, ".lmdl", NULL);
	if (dst == NULL)
	{
		free (src);
		return 1;
	}

	/* Check for modifications. */
	ret = stat (src, &src_st);
	ret |= stat (dst, &dst_st);
	free (src);
	free (dst);
	if (ret != 0)
		return 1;
	if (src_st.st_mtime < dst_st.st_mtime)
		return 0;

	return 1;
}

static int
private_filter_img_modified (const char* dir,
                             const char* name)
{
	int ret;
	char* src;
	char* dst;
	struct stat src_st;
	struct stat dst_st;

	/* Check for extension. */
	if (!lisys_path_check_ext (name, "jpg") &&
	    !lisys_path_check_ext (name, "png"))
		return 0;

	/* Construct file names. */
	src = lisys_path_concat (dir, name, NULL);
	if (src == NULL)
		return 1;
	dst = lisys_path_format (src, LISYS_PATH_STRIPEXTS, ".dds", NULL);
	if (dst == NULL)
	{
		free (src);
		return 1;
	}

	/* Check for modifications. */
	ret = stat (src, &src_st);
	ret |= stat (dst, &dst_st);
	free (src);
	free (dst);
	if (ret != 0)
		return 1;
	if (src_st.st_mtime < dst_st.st_mtime)
		return 0;

	return 1;
}

static int
private_filter_xcf_modified (const char* dir,
                             const char* name)
{
	int ret;
	char* src;
	char* dst;
	struct stat src_st;
	struct stat dst_st;

	/* Check for extension. */
	if (!lisys_path_check_ext (name, "xcf") &&
	    !lisys_path_check_ext (name, "xcf.gz"))
		return 0;

	/* Construct file names. */
	src = lisys_path_concat (dir, name, NULL);
	if (src == NULL)
		return 1;
	dst = lisys_path_format (src, LISYS_PATH_STRIPEXTS, ".dds", NULL);
	if (dst == NULL)
	{
		free (src);
		return 1;
	}

	/* Check for modifications. */
	ret = stat (src, &src_st);
	ret |= stat (dst, &dst_st);
	free (src);
	free (dst);
	if (ret != 0)
		return 1;
	if (src_st.st_mtime < dst_st.st_mtime)
		return 0;

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
	lithr_async_call_stop (self->worker);
}

/**
 * \brief Called every tick to update packager status.
 *
 * If reloading is still in process, updates the progress dialog status.
 * If reloading has ended, frees the progress dialog and the worker thread.
 *
 * \param self Reload.
 */
static void
private_progress_update (liextReload* self)
{
	if (lithr_async_call_get_done (self->worker))
	{
		liwdg_manager_remove_window (self->module->widgets, self->progress);
		lithr_async_call_free (self->worker);
		self->progress = NULL;
		self->worker = NULL;
	}
	else
	{
		liwdg_busy_set_progress (LIWDG_BUSY (self->progress),
			lithr_async_call_get_progress (self->worker));
	}
}

/** @} */
/** @} */
/** @} */

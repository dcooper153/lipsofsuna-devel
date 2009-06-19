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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup liengRender Render
 * @{
 */

#ifndef LIENG_DISABLE_GRAPHICS

#include <dlfcn.h>
#include "engine-render.h"

liengRender*
lieng_render_new (const char* dir)
{
	liengRender* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liengRender));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Resolve symbols. */
	void* prog = dlopen (NULL, RTLD_NOW);
#define SYM(a) \
	self->a = dlsym (prog, #a); \
	if (self->a == NULL) \
	{ \
		lisys_error_set (ENOENT, "cannot find symbol `%s'", #a); \
		free (self); \
		return NULL; \
	}
	SYM (lirnd_model_new);
	SYM (lirnd_model_free);
	SYM (lirnd_object_new);
	SYM (lirnd_object_free);
	SYM (lirnd_object_deform);
	SYM (lirnd_object_replace_image);
	SYM (lirnd_object_update);
	SYM (lirnd_object_set_model);
	SYM (lirnd_object_set_realized);
	SYM (lirnd_object_set_transform);
	SYM (lirnd_render_new);
	SYM (lirnd_render_free);
	SYM (lirnd_render_update);
	SYM (lirnd_render_find_image);
	SYM (lirnd_render_load_image);
#undef SYM

	/* Allocate renderer. */
	self->render = self->lirnd_render_new (dir);
	if (self->render == NULL)
	{
		free (self);
		return NULL;
	}

	return self;
}

void
lieng_render_free (liengRender* self)
{
	self->lirnd_render_free (self->render);
	free (self);
}

#endif

/** @} */
/** @} */

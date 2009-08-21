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
 * \addtogroup livie Viewer
 * @{
 * \addtogroup livieViewer Viewer
 * @{
 */

#ifndef __VIEWER_H__
#define __VIEWER_H__

#include <SDL/SDL.h>
#include <engine/lips-engine.h>
#include <paths/lips-paths.h>
#include <reload/lips-reload.h>
#include <system/lips-system.h>
#include <video/lips-video.h>

typedef struct _livieViewer livieViewer;
struct _livieViewer
{
	SDL_Surface* screen;
	liengCamera* camera;
	liengEngine* engine;
	liengObject* object;
	lipthPaths* paths;
	lirelReload* reload;
	lividCalls video;
	struct
	{
		lirndLight* key;
		lirndLight* fill;
	} lights;
	struct
	{
		int width;
		int height;
		int fsaa;
	} mode;
};

livieViewer*
livie_viewer_new (lividCalls* video,
                  lipthPaths* paths,
                  const char* model);

void
livie_viewer_free (livieViewer* self);

int
livie_viewer_main (livieViewer* self);

#endif

/** @} */
/** @} */

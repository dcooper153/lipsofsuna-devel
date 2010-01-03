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
 * \addtogroup livie Viewer
 * @{
 * \addtogroup LIVieViewer Viewer
 * @{
 */

#ifndef __VIEWER_H__
#define __VIEWER_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/paths.h>
#include <lipsofsuna/reload.h>
#include <lipsofsuna/render.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/video.h>

typedef struct _LIVieViewer LIVieViewer;
struct _LIVieViewer
{
	char* file;
	LIAlgCamera* camera;
	LIPthPaths* paths;
	LIRelReload* reload;
	LIRenModel* model;
	LIRenObject* object;
	LIRenRender* render;
	LIRenScene* scene;
	LIVidCalls video;
	SDL_Surface* screen;
	struct
	{
		LIRenLight* key;
		LIRenLight* fill;
	} lights;
	struct
	{
		int width;
		int height;
		int fsaa;
	} mode;
};

LIVieViewer*
livie_viewer_new (LIVidCalls* video,
                  LIPthPaths* paths,
                  const char* model);

void
livie_viewer_free (LIVieViewer* self);

int
livie_viewer_main (LIVieViewer* self);

#endif

/** @} */
/** @} */

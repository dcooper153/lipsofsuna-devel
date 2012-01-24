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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtHeightmapRender HeightmapRender
 * @{
 */

#include "module.h"
#include "lipsofsuna/render/internal/render-internal.h"
#include <OgreTerrainGroup.h>

static void private_init_heightmap (
	LIExtHeightmapRender* self,
	LIExtHeightmap*       heightmap);

static void private_free_heightmap (
	LIExtHeightmapRender* self,
	LIExtHeightmap*       heightmap);

/*****************************************************************************/

extern "C"
{
	LIMaiExtensionInfo liext_heightmap_render_info =
	{
		LIMAI_EXTENSION_VERSION, "HeightmapRender",
		(void*) liext_heightmap_render_new,
		(void*) liext_heightmap_render_free
	};
}

LIExtHeightmapRender* liext_heightmap_render_new (
	LIMaiProgram* program)
{
	LIExtHeightmapRender* self;

	/* Allocate self. */
	self = (LIExtHeightmapRender*) lisys_calloc (1, sizeof (LIExtHeightmapRender));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Make sure the needed extensions are loaded. */
	if (!limai_program_insert_extension (program, "heightmap") ||
	    !limai_program_insert_extension (program, "render"))
	{
		liext_heightmap_render_free (self);
		return NULL;
	}

	/* Find the required components. */
	self->heightmap = (LIExtHeightmapModule*) limai_program_find_component (program, "heightmap");
	self->render = (LIRenRender*) limai_program_find_component (program, "render");
	if (self->heightmap == NULL || self->render == NULL)
	{
		liext_heightmap_render_free (self);
		return NULL;
	}

	/* Add the heightmap hook. */
	lisys_assert (self->heightmap->render_hooks == NULL);
	self->hooks.data = self;
	self->hooks.init = (LIExtHeightmapInitFunc) private_init_heightmap;
	self->hooks.free = (LIExtHeightmapFreeFunc) private_free_heightmap;
	self->heightmap->render_hooks = &self->hooks;

	/* Setup default terrain options. */
	self->globals = OGRE_NEW Ogre::TerrainGlobalOptions ();
	self->globals->setMaxPixelError (8);
	self->globals->setCompositeMapDistance (3000);
	self->globals->setLightMapDirection (Ogre::Vector3 (0.0f, -1.0f, 0.0f));
	self->globals->setCompositeMapAmbient (Ogre::ColourValue (0.9f, 0.9f, 0.9f));
	self->globals->setCompositeMapDiffuse (Ogre::ColourValue (1.0f, 1.0f, 1.0f));

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_HEIGHTMAP_RENDER, self);
	liext_script_heightmap_render (program->script);

	return self;
}

void liext_heightmap_render_free (
	LIExtHeightmapRender* self)
{
	LIAlgU32dicIter iter;
	LIExtHeightmap* heightmap;

	if (self->heightmap != NULL)
	{
		/* Free existing render data. */
		LIALG_U32DIC_FOREACH (iter, self->heightmap->heightmaps)
		{
			heightmap = (LIExtHeightmap*) iter.value;
			if (heightmap->render_data != NULL)
				private_free_heightmap (self, heightmap);
		}

		/* Remove heightmap hooks. */
		self->heightmap->render_hooks = NULL;
	}

	delete self->globals;
	lisys_free (self);
}

/*****************************************************************************/

static void private_init_heightmap (
	LIExtHeightmapRender* self,
	LIExtHeightmap*       heightmap)
{
	/* Specify the blend map size. */
	/* We want the blend map to be of the same size with the terrain
	   image instead of whatever default value Ogre happens to use. */
	self->globals->setLayerBlendMapSize (heightmap->size);

	/* Create the terrain group. */
	float width = (heightmap->size - 1) * heightmap->spacing;
	Ogre::TerrainGroup* group = OGRE_NEW Ogre::TerrainGroup (self->render->data->scene_manager,
		Ogre::Terrain::ALIGN_X_Z, heightmap->size, width);
	Ogre::Terrain::ImportData& settings = group->getDefaultImportSettings ();
	settings.terrainSize = heightmap->size;
	settings.worldSize = width;
	settings.inputScale = 1;
	settings.minBatchSize = 33;
	settings.maxBatchSize = 65;

	/* Convert the heightmap. */
	/* Bullet and Ogre use a different heightmap scheme so we need to convert
	   the heightmap for one or the other. We do the conversion for Ogre since
	   it makes an internal copy but Bullet doesn't. Bullet is also used in
	   both the server and the client so we get one less conversion when running
	   a single player game. */
	float* heights = new float[heightmap->size * heightmap->size];
	for (int i = 0 ; i < heightmap->size ; i++)
	{
		memcpy (heights + heightmap->size * i,
			heightmap->heights + heightmap->size * (heightmap->size - i - 1),
			heightmap->size * sizeof (float));
	}

	/* Setup the geometry. */
	Ogre::Vector3 pos (heightmap->position.x, heightmap->position.y, heightmap->position.z);
	group->setOrigin (pos);
	group->defineTerrain (0, 0, heights);
	group->loadAllTerrains (true);
	group->freeTemporaryResources ();

	/* Free the converted heights. */
	delete[] heights;

	/* Register the render data. */
	heightmap->render_data = group;
}

static void private_free_heightmap (
	LIExtHeightmapRender* self,
	LIExtHeightmap*       heightmap)
{
	if (heightmap->render_data != NULL)
	{
		Ogre::TerrainGroup* group = (Ogre::TerrainGroup*) heightmap->render_data;
		delete group;
		heightmap->render_data = NULL;
	}
}

/** @} */
/** @} */

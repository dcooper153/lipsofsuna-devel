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
 * \addtogroup LIExtHeightmap Heightmap
 * @{
 */

#include "heightmap.h"
#include "lipsofsuna/render/internal/render-internal.h"
#include <OgreTerrainGroup.h>

/**
 * \brief Creates a new texture layer.
 * \param self Heightmap.
 * \param name Unique layer name.
 * \param diffuse Image filename.
 * \param specular Image filename.
 * \param normal Image filename.
 * \param height Image filename.
 * \param blend Image filename, or NULL.
 */
int liext_heightmap_add_texture_layer (
	LIExtHeightmap* self,
	float           size,
	const char*     name,
	const char*     diffuse,
	const char*     specular,
	const char*     normal,
	const char*     height,
	const char*     blend)
{
	const char* path;
	Ogre::String fmt = ".png";
	LIExtHeightmapRender* module = (LIExtHeightmapRender*) self->module->render_hooks->data;

	/* Create the diffuse-specular texture. */
	Ogre::String dsname = Ogre::String (name) + "ds" + fmt;
	try
	{
		Ogre::Image ds;
		ds.loadTwoImagesAsRGBA (diffuse + fmt, specular + fmt, LIREN_RESOURCES_PERMANENT, Ogre::PF_BYTE_RGBA);
		path = lipth_paths_find_file (module->render->paths, dsname.c_str ());
		ds.save (path);
	}
	catch (Ogre::Exception& e)
	{
		lisys_error_set (EINVAL, "cannot create the diffuse-specular texture: %s", e.getDescription ().c_str ());
		return 0;
	}

	/* Create the normal-height texture. */
	Ogre::String nhname = Ogre::String (name) + "nh" + fmt;
	try
	{
		Ogre::Image nh;
		nh.loadTwoImagesAsRGBA (normal + fmt, height + fmt, LIREN_RESOURCES_PERMANENT, Ogre::PF_BYTE_RGBA);
		path = lipth_paths_find_file (module->render->paths, nhname.c_str ());
		nh.save (path);
	}
	catch (Ogre::Exception& e)
	{
		lisys_error_set (EINVAL, "cannot create the normal-height texture: %s", e.getDescription ().c_str ());
		return 0;
	}

	/* Create the texture layer. */
	Ogre::TerrainGroup* group = (Ogre::TerrainGroup*) self->render_data;
	Ogre::Terrain* terrain = group->getTerrain (0, 0);
	int layer = terrain->getLayerCount ();
	Ogre::vector<Ogre::String>::type textures;
	textures.push_back (dsname);
	textures.push_back (nhname);
	terrain->addLayer (layer, size, &textures);

	/* Create the blend map. */
	if (layer && blend != NULL)
	{
		try
		{
			/* Load the blend map image. */
			/* Ogre provides a function for loading the image directly to the blend
			   map but it doesn't work correctly with ordinary 8-bit PNGs. We want
			   to use common file formats so we load and convert the image manually. */
			Ogre::TerrainLayerBlendMap* blendmap = terrain->getLayerBlendMap (layer);
			Ogre::Image bm;
			bm.load (blend + fmt, LIREN_RESOURCES_PERMANENT);

			/* Make sure that the size is correct. */
			size_t width = terrain->getLayerBlendMapSize ();
			if (width != bm.getWidth () || width != bm.getHeight ())
				throw Ogre::Exception (0, "Invalid blend map size", "");

			/* Copy pixels from the image to the blend map. */
			float* ptr = blendmap->getBlendPointer ();
			for (size_t y = 0 ; y < width ; y++)
			for (size_t x = 0 ; x < width ; x++)
			{
				*ptr = bm.getColourAt (x, y, 0).r;
				ptr++;
			}

			/* Refresh the blend map. */
			blendmap->dirty();
			blendmap->update();
		}
		catch (Ogre::Exception& e)
		{
			lisys_error_set (EINVAL, "cannot create the blend map texture: %s", e.getDescription ().c_str ());
			return 0;
		}
	}

	return 1;
}

/**
 * \brief Rebuilds the heightmap.
 * \param self Heightmap.
 */
void liext_heightmap_rebuild (
	LIExtHeightmap* self)
{
	Ogre::TerrainGroup* group = (Ogre::TerrainGroup*) self->render_data;
	Ogre::Terrain* terrain = group->getTerrain (0, 0);
	terrain->dirty ();
	terrain->update ();
}

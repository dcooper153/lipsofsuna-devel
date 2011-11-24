/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenOverlay Overlay
 * @{
 */

#include "render-internal.h"

static bool private_create_material (
	LIRenOverlay*      self,
	const char*        material_name,
	Ogre::MaterialPtr* material_result,
	Ogre::TexturePtr*  texture_result);

static void private_remove_overlay (
	LIRenOverlay* self,
	LIRenOverlay* child);

static Ogre::String private_unique_element (
	LIRenOverlay* self);

static Ogre::String private_unique_overlay (
	LIRenOverlay* self);

static void private_update_position (
	LIRenOverlay* self);

/*****************************************************************************/

/**
 * \brief Creates a new overlay.
 * \param render Renderer.
 * \return Overlay or NULL on failure.
 */
LIRenOverlay* liren_overlay_new (
	LIRenRender* render)
{
	LIRenOverlay* self;

	/* Allocate self. */
	self = (LIRenOverlay*) lisys_calloc (1, sizeof (LIRenOverlay));
	if (self == NULL)
		return 0;
	self->render = render;

	/* Choose a unique ID. */
	while (!self->id)
	{
		self->id = lialg_random_range (&render->random, 0x00000000, 0x0FFFFFFF);
		if (lialg_u32dic_find (render->overlays, self->id))
			self->id = 0;
	}

	/* Add to the dictionary. */
	if (!lialg_u32dic_insert (render->overlays, self->id, self))
	{
		liren_overlay_free (self);
		return 0;
	}

	/* Initialize the private data. */
	self->data = (LIRenOverlayData*) lisys_calloc (1, sizeof (LIRenOverlayData));
	if (self->data == NULL)
	{
		liren_overlay_free (self);
		return 0;
	}
	self->data->container = (Ogre::OverlayContainer*) render->data->overlay_manager->createOverlayElement ("LIRenContainer", private_unique_overlay (self));
	self->data->container->setMetricsMode (Ogre::GMM_PIXELS);

	return self;
}

/**
 * \brief Frees the overlay.
 * \param self Overlay.
 */
void liren_overlay_free (
	LIRenOverlay* self)
{
	int i;

	/* Detach children. */
	for (i = 0 ; i < self->overlays.count ; i++)
		self->overlays.array[i]->parent = NULL;

	/* Remove from the scene. */
	if (self->parent != NULL)
		private_remove_overlay (self->parent, self);
	lialg_u32dic_remove (self->render->overlays, self->id);

	/* Free scene data. */
	if (self->scene.framebuffer != NULL)
		liren_framebuffer_free (self->scene.framebuffer);
	lisys_free (self->scene.render_passes);
	lisys_free (self->scene.postproc_passes);

	/* Free private data. */
	if (self->data != NULL)
	{
		liren_overlay_clear (self);
		if (self->data->overlay != NULL)
		{
			self->data->overlay->remove2D (self->data->container);
			self->render->data->overlay_manager->destroy (self->data->container->getName ());
		}
		if (self->data->container != NULL)
			self->render->data->overlay_manager->destroyOverlayElement (self->data->container);
		lisys_free (self->data);
	}

	lisys_free (self);
}

/**
 * \brief Clears the elements of the overlay.
 * \param self Overlay.
 */
void liren_overlay_clear (
	LIRenOverlay* self)
{
	size_t i;
	Ogre::OverlayElement* elem;

	/* Free elements. */
	for (i = 0 ; i < self->data->elements.size () ; i++)
	{
		elem = self->data->elements[i];
		Ogre::String name (elem->getName ());
		self->data->container->removeChild (name);
		self->render->data->overlay_manager->destroyOverlayElement (name);
	}
	self->data->elements.clear ();
}

/**
 * \brief Adds text to the overlay.
 * \param self Overlay.
 * \param shader Shader name.
 * \param font Font name.
 * \param color Diffuse color.
 * \param scissor Screen space scissor rectangle.
 * \param pos Text offset relative to the overlay origin.
 * \param size Clip rectangle size.
 * \param align Text alignment.
 */
void liren_overlay_add_text (
	LIRenOverlay* self,
	const char*   shader,
	const char*   font,
	const char*   text,
	const float*  color,
	const int*    scissor,
	const int*    pos,
	const int*    size,
	const float*  align)
{
	LIFntFont* font_;

	/* Get the font. */
	font_ = (LIFntFont*) lialg_strdic_find (self->render->fonts, font);
	if (font_ == NULL)
		return;

	/* Create the element. */
	Ogre::String id = private_unique_element (self);
	Ogre::TextAreaOverlayElement* elem = (Ogre::TextAreaOverlayElement*) self->render->data->overlay_manager->createOverlayElement ("TextArea", id);
	elem->setMetricsMode (Ogre::GMM_PIXELS);
	elem->setPosition (pos[0], pos[1]);
	elem->setDimensions (size[0], size[1]);
	elem->setCharHeight (lifnt_font_get_height (font_));
	elem->setFontName (font);
	elem->setColour (Ogre::ColourValue (color[0], color[1], color[2], color[3]));
	elem->setCaption (text);
	elem->show ();

	/* Add to the list. */
	self->data->elements.push_back (elem);
	self->data->container->addChild (elem);
}

/**
 * \brief Adds a tiled image to the overlay.
 * \param self Overlay.
 * \param material_name Material name.
 * \param dest_position Position in pixels relative to the parent.
 * \param dest_size Size in pixels.
 * \param source_position Position in pixels in the source image.
 * \param source_tiling Tiling pattern in pixels.
 */
void liren_overlay_add_tiled (
	LIRenOverlay* self,
	const char*   material_name,
	const int*    dest_position,
	const int*    dest_size,
	const int*    source_position,
	const int*    source_tiling)
{
	/* Get the texture size. */
	Ogre::MaterialPtr material;
	Ogre::TexturePtr texture;
	if (!private_create_material (self, material_name, &material, &texture))
		return;
	Ogre::Real w = texture->getWidth ();
	Ogre::Real h = texture->getHeight ();

	/* Create a new border panel overlay. */
	Ogre::String id = private_unique_element (self);
	Ogre::BorderPanelOverlayElement* elem = (Ogre::BorderPanelOverlayElement*) self->render->data->overlay_manager->createOverlayElement ("BorderPanel", id); 
	elem->setMetricsMode (Ogre::GMM_PIXELS);
	elem->setPosition (dest_position[0], dest_position[1]);
	elem->setDimensions (dest_size[0], dest_size[1]);
	elem->setMaterialName (material_name);
	elem->setBorderMaterialName (material_name);
	elem->setBorderSize (source_tiling[0], source_tiling[2], source_tiling[3], source_tiling[5]);

	/* Set texture coordinates. */
	float x[4] =
	{
		source_position[0],
		source_position[0] + source_tiling[0],
		source_position[0] + source_tiling[0] + source_tiling[1],
		source_position[0] + source_tiling[0] + source_tiling[1] + source_tiling[2]
	};
	float y[4] =
	{
		source_position[1],
		source_position[1] + source_tiling[3],
		source_position[1] + source_tiling[3] + source_tiling[4],
		source_position[1] + source_tiling[3] + source_tiling[4] + source_tiling[5]
	};
	elem->setTopLeftBorderUV     (x[0] / w, y[0] / h, x[1] / w, y[1] / h);
	elem->setTopBorderUV         (x[1] / w, y[0] / h, x[2] / w, y[1] / h);
	elem->setTopRightBorderUV    (x[2] / w, y[0] / h, x[3] / w, y[1] / h);
	elem->setLeftBorderUV        (x[0] / w, y[1] / h, x[1] / w, y[2] / h);
	elem->setUV                  (x[1] / w, y[1] / h, x[2] / w, y[2] / h);
	elem->setRightBorderUV       (x[2] / w, y[1] / h, x[3] / w, y[2] / h);
	elem->setBottomLeftBorderUV  (x[0] / w, y[2] / h, x[1] / w, y[3] / h);
	elem->setBottomBorderUV      (x[1] / w, y[2] / h, x[2] / w, y[3] / h);
	elem->setBottomRightBorderUV (x[2] / w, y[2] / h, x[3] / w, y[3] / h);
	elem->show ();

	/* Add to the list. */
	self->data->elements.push_back (elem);
	self->data->container->addChild (elem);
}

/**
 * \brief Adds a tiled image to the overlay.
 * \param self Overlay.
 * \param material_name Material name.
 * \param dest_position Position in pixels relative to the parent.
 * \param dest_size Size in pixels.
 * \param source_position Position in pixels in the source image.
 * \param source_tiling Tiling pattern in pixels.
 */
void liren_overlay_add_scaled (
	LIRenOverlay* self,
	const char*   material_name,
	const int*    dest_position,
	const int*    dest_size,
	const int*    source_position,
	const int*    source_tiling)
{
	/* Get the texture size. */
	Ogre::MaterialPtr material;
	Ogre::TexturePtr texture;
	if (!private_create_material (self, material_name, &material, &texture))
		return;
	Ogre::Real w = texture->getWidth ();
	Ogre::Real h = texture->getHeight ();

	/* Calculate texture coordinates. */
	float tiling[2] =
	{
		source_tiling[1] >= 0? source_tiling[1] : w,
		source_tiling[4] >= 0? source_tiling[4] : h
	};
	float tx[2] =
	{
		(float)(source_position[0]) / w,
		(float)(source_position[0] + tiling[0]) / w
	};
	float ty[2] =
	{
		(float)(source_position[1]) / h,
		(float)(source_position[1] + tiling[1]) / h
	};

	/* Calculate pixels per texture unit. */
	float xs = tx[1] - tx[0];
	float ys = ty[1] - ty[0];
	if (xs < LIMAT_EPSILON || ys < LIMAT_EPSILON)
		return;
	xs = dest_size[0] / xs;
	ys = dest_size[1] / ys;

	/* Scale and translate to fill the area. */
	float center;
	float size;
	if (ty[1] - ty[0] >= dest_size[1] / xs)
	{
		center = 0.5f * (ty[0] + ty[1]);
		size = ty[1] - ty[0];
		ty[0] = center - 0.5f * dest_size[1] / xs;
		ty[1] = center + 0.5f * dest_size[1] / xs;
	}
	else
	{
		center = 0.5f * (tx[0] + tx[1]);
		size = tx[1] - tx[0];
		tx[0] = center - 0.5f * dest_size[0] / ys;
		tx[1] = center + 0.5f * dest_size[0] / ys;
	}

	/* Create a new panel overlay. */
	Ogre::String id = private_unique_element (self);
	Ogre::PanelOverlayElement* elem = (Ogre::PanelOverlayElement*) self->render->data->overlay_manager->createOverlayElement ("Panel", id); 
	elem->setMetricsMode (Ogre::GMM_PIXELS);
	elem->setPosition (dest_position[0], dest_position[1]);
	elem->setDimensions (dest_size[0], dest_size[1]);
	elem->setMaterialName (material_name);
	elem->setUV (tx[0], ty[0], tx[1], ty[1]);
	elem->show ();

	/* Add to the list. */
	self->data->elements.push_back (elem);
	self->data->container->addChild (elem);
}

/**
 * \brief Adds a child overlay to the overlay.
 * \param self Overlay.
 * \param overlay Overlay ID.
 */
void liren_overlay_add_overlay (
	LIRenOverlay* self,
	LIRenOverlay* overlay)
{
	LIRenOverlay** tmp;

	/* Detach the overlay. */
	if (overlay->parent == self)
		return;
	if (overlay->parent != NULL)
		private_remove_overlay (overlay->parent, overlay);

	/* Attach the overlay. */
	tmp = (LIRenOverlay**) lisys_realloc (self->overlays.array, (self->overlays.count + 1) * sizeof (LIRenOverlay*));
	if (tmp == NULL)
		return;
	tmp[self->overlays.count] = overlay;
	self->overlays.array = tmp;
	self->overlays.count++;
	overlay->parent = self;

	/* Add to container. */
	self->data->container->addChild (overlay->data->container);
	private_update_position (overlay);
}

/**
 * \brief Removes a child overlay from the overlay.
 * \param self Overlay.
 * \param overlay Overlay ID.
 */
void liren_overlay_remove_overlay (
	LIRenOverlay* self,
	LIRenOverlay* overlay)
{
	/* Detach the overlay. */
	if (overlay->parent == self)
		private_remove_overlay (self, overlay);
}

/**
 * \brief Disables scene rendering for the overlay.
 * \param self Overlay.
 */
void liren_overlay_disable_scene (
	LIRenOverlay* self)
{
	/* Disable the scene. */
	self->scene.enabled = 0;
	if (self->scene.framebuffer != NULL)
	{
		liren_framebuffer_free (self->scene.framebuffer);
		self->scene.framebuffer = NULL;
	}
	lisys_free (self->scene.render_passes);
	self->scene.render_passes = NULL;
	self->scene.render_passes_num = 0;
	lisys_free (self->scene.postproc_passes);
	self->scene.postproc_passes = NULL;
	self->scene.postproc_passes_num = 0;
}

/**
 * \brief Enables scene rendering for the overlay.
 * \param self Overlay.
 * \param samples Number of multisamples.
 * \param hdr Nonzero to enable HDR.
 * \param viewport Viewport array of the camera.
 * \param modelview Modelview matrix of the camera.
 * \param projection Projection matrix of the camera.
 * \param frustum Frustum of the camera.
 * \param render_passes Array of render passes.
 * \param render_passes_num Number of render passes.
 * \param postproc_passes Array of post-processing passes.
 * \param postproc_passes_num Number of post-processing passes.
 */
void liren_overlay_enable_scene (
	LIRenOverlay*      self,
	int                samples,
	int                hdr,
	const int*         viewport,
	LIMatMatrix*       modelview,
	LIMatMatrix*       projection,
	LIMatFrustum*      frustum,
	LIRenPassRender*   render_passes,
	int                render_passes_num,
	LIRenPassPostproc* postproc_passes,
	int                postproc_passes_num)
{
	/* Copy camera settings. */
	self->scene.enabled = 1;
	self->scene.viewport[0] = viewport[0];
	self->scene.viewport[1] = viewport[1];
	self->scene.viewport[2] = viewport[2];
	self->scene.viewport[3] = viewport[3];
	self->scene.modelview = *modelview;
	self->scene.projection = *projection;
	self->scene.frustum = *frustum;
	self->scene.samples = samples;
	self->scene.hdr = hdr;

	/* Copy render passes. */
	lisys_free (self->scene.render_passes);
	self->scene.render_passes = NULL;
	self->scene.render_passes_num = 0;
	if (render_passes_num)
	{
		self->scene.render_passes = (LIRenPassRender*) lisys_calloc (render_passes_num, sizeof (LIRenPassRender));
		if (self->scene.render_passes != NULL)
		{
			memcpy (self->scene.render_passes, render_passes, render_passes_num * sizeof (LIRenPassRender));
			self->scene.render_passes_num = render_passes_num;
		}
	}

	/* Copy post-processing passes. */
	lisys_free (self->scene.postproc_passes);
	self->scene.postproc_passes = NULL;
	self->scene.postproc_passes_num = 0;
	if (postproc_passes_num)
	{
		self->scene.postproc_passes = (LIRenPassPostproc*) lisys_calloc (postproc_passes_num, sizeof (LIRenPassPostproc));
		if (self->scene.postproc_passes != NULL)
		{
			memcpy (self->scene.postproc_passes, postproc_passes, postproc_passes_num * sizeof (LIRenPassPostproc));
			self->scene.postproc_passes_num = postproc_passes_num;
		}
	}
}

/**
 * \brief Sets the overlay behind other overlays.
 * \param self Overlay.
 * \param value Nonzero for behind.
 */
void liren_overlay_set_depth (
	LIRenOverlay* self,
	int           value)
{
	if (self->depth == value)
		return;
	self->depth = value;
	if (self->data->overlay != NULL)
		self->data->overlay->setZOrder (self->depth);
}

void liren_overlay_set_floating (
	LIRenOverlay* self,
	int           value)
{
	Ogre::String id (private_unique_overlay (self));
	if (value && self->data->overlay == NULL)
	{
		self->data->overlay = self->render->data->overlay_manager->create (id);
		self->data->overlay->add2D (self->data->container);
		self->data->overlay->setZOrder (self->depth);
		if (self->visible)
			self->data->overlay->show ();
		private_update_position (self);
	}
	else if (!value && self->data->overlay != NULL)
	{
		self->data->overlay->remove2D (self->data->container);
		self->render->data->overlay_manager->destroy (id);
		self->data->overlay = NULL;
	}
}

/**
 * \brief Sets the screen position of the overlay.
 * \param self Overlay.
 * \param value Position vector.
 */
void liren_overlay_set_position (
	LIRenOverlay*      self,
	const LIMatVector* value)
{
	self->position = *value;
	private_update_position (self);
}

/**
 * \brief Sets the visibility of the overlay.
 * \param self Overlay.
 * \param value Nonzero for visible.
 */
void liren_overlay_set_visible (
	LIRenOverlay* self,
	int           value)
{
	self->visible = value;
	if (value)
	{
		self->data->container->show ();
		private_update_position (self);
	}
	else
		self->data->container->hide ();
	if (self->data->overlay != NULL)
	{
		if (value)
			self->data->overlay->show ();
		else
			self->data->overlay->hide ();
	}
}

/*****************************************************************************/

static bool private_create_material (
	LIRenOverlay*      self,
	const char*        material_name,
	Ogre::MaterialPtr* material_result,
	Ogre::TexturePtr*  texture_result)
{
	/* Load or create the material. */
	const Ogre::String& group = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
	Ogre::MaterialPtr material = self->render->data->material_manager->load (material_name, group);
	if (material.isNull ())
		return false;

	/* Get the first technique. */
	if (!material->getNumTechniques ())
		return false;
	Ogre::Technique* technique = material->getTechnique (0);

	/* Get the first pass. */
	if (!technique->getNumPasses ())
		return false;
	Ogre::Pass* pass = technique->getPass (0);

	/* Get or create the texture unit. */
	Ogre::TextureUnitState* unit;
	if (!pass->getNumTextureUnitStates ())
	{
		if (pass->isProgrammable ())
			return false;
		material->setSceneBlending (Ogre::SBT_TRANSPARENT_ALPHA);
		unit = pass->createTextureUnitState (Ogre::String (material_name) + ".dds");
	}
	else
		unit = pass->getTextureUnitState (0);

	/* Get the texture. */
	const Ogre::String& texname = unit->getTextureName ();
	Ogre::TexturePtr texture = self->render->data->texture_manager->load (texname, group);
	if (texture.isNull ())
		return false;

	/* Return the results. */
	*material_result = material;
	*texture_result = texture;

	return true;
}

static void private_remove_overlay (
	LIRenOverlay* self,
	LIRenOverlay* child)
{
	int i;

	for (i = 0 ; i < self->overlays.count ; i++)
	{
		if (self->overlays.array[i] == child)
		{
			/* Remove from the array. */
			if (self->overlays.count)
			{
				for ( ; i < self->overlays.count - 1 ; i++)
					self->overlays.array[i] = self->overlays.array[i + 1];
				self->overlays.count--;
			}
			else
			{
				lisys_free (self->overlays.array);
				self->overlays.array = NULL;
				self->overlays.count = 0;
			}

			/* Detach the child. */
			child->parent = NULL;
			self->data->container->removeChild (private_unique_overlay (child));

			/* Remove the overlay. */
			if (child->data->overlay != NULL)
			{
				child->data->overlay->remove2D (child->data->container);
				child->render->data->overlay_manager->destroy (private_unique_overlay (child));
				child->data->overlay = NULL;
			}
			break;
		}
	}
}

static Ogre::String private_unique_element (
	LIRenOverlay* self)
{
	Ogre::String id1(Ogre::StringConverter::toString (self->id));
	Ogre::String id2(Ogre::StringConverter::toString (self->data->elements.size ()));
	return id1 + "." + id2;
}

static Ogre::String private_unique_overlay (
	LIRenOverlay* self)
{
	Ogre::String id1(Ogre::StringConverter::toString (self->id));
	return id1;
}

static void private_update_position (
	LIRenOverlay* self)
{
	float x = self->position.x;
	float y = self->position.y;

	if (self->parent != NULL)
	{
		x -= self->parent->position.x;
		y -= self->parent->position.y;
	}
	self->data->container->setPosition (x, y);
}

/** @} */
/** @} */
/** @} */

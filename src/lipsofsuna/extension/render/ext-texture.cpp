#include "ext-module.h"
#include "lipsofsuna/render/internal/render.hpp"
#include "../image/module.h"

#include <OgreTextureManager.h>

typedef struct _LIExtTexture LIExtTexture;
struct _LIExtTexture
{
	LIExtModule* module;
	char *name;
	Ogre::TexturePtr texture;
};


static void Texture_free(LIExtTexture* self)
{
	/*Release the texture pointer.*/
	self->texture.setNull();
	/*Release name.*/
	if(self->name != NULL) {
		lisys_free(self->name);
		self->name = NULL;
	}
	/*Call the destructor on the object.*/
	self->~_LIExtTexture();
	/*And finally free the memory.*/
	lisys_free(self);
}

static void Texture_new(LIScrArgs* args)
{
	LIExtModule* module;
	const char* name;
	LIExtTexture *self;
	LIScrData* data;

	/*Get the name*/
	if (!liscr_args_geti_string (args, 0, &name)) {
		return;
	}

	/*Allocate self.*/
	module = (LIExtModule *)liscr_script_get_userdata (args->script, LIEXT_SCRIPT_TEXTURE);
	self = (LIExtTexture *)lisys_calloc (1, sizeof (LIExtTexture));
	if (self == NULL) {
		return;
	}
	/* Construct the struct, so the TexturePtr class is initialised.*/
	new(self) _LIExtTexture();
	self->module = module;

	/*Copy the name.*/
	self->name = lisys_string_dup(name);
	if(!self->name) {
		Texture_free(self);
		return;
	}

	/*Find or allocate the texture.*/
	self->texture = module->render->texture_manager->getByName(name);
	if(self->texture.isNull()) {
		self->texture = module->render->texture_manager->create(name, LIREN_RESOURCES_PERMANENT, true);
		if(self->texture.isNull()) {
			Texture_free(self);
			return;
		}
	}

	/* Allocate userdata. */
	data = (LIScrData *)liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_TEXTURE, (LIScrGCFunc)Texture_free);
	if (data == NULL) {
		Texture_free(self);
		return;
	}

	liscr_args_seti_stack (args);
}

static void Texture_set_image(LIScrArgs* args)
{
	LIExtTexture *self = (LIExtTexture *)args->self;
	LIScrData* value;
	LIImgImage* image;

	if (!liscr_args_geti_data (args, 0, LIEXT_SCRIPT_IMAGE, &value)) {
		return;
	}
	image = (LIImgImage *)liscr_data_get_data (value);
	Ogre::Image img;
	img.loadDynamicImage ((Ogre::uchar*) image->pixels, image->width, image->height, 1, Ogre::PF_A8B8G8R8);
	/*Unload the old image, otherwise it will not update the texture if an image has already been loaded.*/
	self->texture->unload();
	/*And load in the new one.*/
	self->texture->loadImage(img);
	//self->module->render->texture_manager->loadImage(self->name, LIREN_RESOURCES_PERMANENT, img, Ogre::TEX_TYPE_2D, 0);
}

void liext_script_texture (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_TEXTURE, "texture_new", Texture_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TEXTURE, "texture_set_image", Texture_set_image);
}

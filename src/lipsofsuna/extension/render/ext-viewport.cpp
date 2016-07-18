#include "ext-module.h"
#include "lipsofsuna/render/internal/render.hpp"
#include "../image/module.h"

static void Viewport_add(LIScrArgs* args)
{
	LIExtModule* module;
	const char* name;
	float view_rect[4] = {0.0f, 0.0f, 1.0f, 1.0f};
	float camera_pos[3] = {0.0f, 0.0f, 0.0f};

	/*Get the extension module.*/
	module = (LIExtModule *)liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VIEWPORT);

	/*Get the name*/
	if (!liscr_args_geti_string (args, 0, &name)) {
		return;
	}
	for(size_t i = 0; i < 4; i++) {
		liscr_args_geti_float (args, i + 1, view_rect + i);
	}
	for(size_t i = 0; i < 3; i++) {
		liscr_args_geti_float (args, i + 5, camera_pos + i);
	}
	module->render->add_viewport(name, view_rect, camera_pos);
}

static void Viewport_remove(LIScrArgs* args)
{
	LIExtModule* module;
	const char* name;

	/*Get the extension module.*/
	module = (LIExtModule *)liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VIEWPORT);

	/*Get the name*/
	if (!liscr_args_geti_string (args, 0, &name)) {
		return;
	}

	module->render->remove_viewport(name);
}

static void Viewport_remove_all(LIScrArgs* args)
{
	LIExtModule* module;


	/*Get the extension module.*/
	module = (LIExtModule *)liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VIEWPORT);

	module->render->remove_all_viewports();
}


void liext_script_viewport (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VIEWPORT, "viewport_add", Viewport_add);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VIEWPORT, "viewport_remove", Viewport_remove);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VIEWPORT, "viewport_remove_all", Viewport_remove_all);
}

#include "physics-debug-module.h"
#include "render-physics-debug.hpp"
#include "lipsofsuna/render/internal/render.hpp"
#include "lipsofsuna/render/render.h"
#include "lipsofsuna/extension/render/ext-module.h"
#include "lipsofsuna/extension/physics/physics.h"
#include "lipsofsuna/extension/physics/physics-private.h"
#include "lipsofsuna/extension/physics/ext-module.h"

typedef struct _LIExtPhysicsDebug LIExtPhysicsDebug;
struct _LIExtPhysicsDebug
{
	LIExtPhysicsDebugModule *module;
	LIPhyPhysics *physics;
	LIRenRender *render;
	OgreDebugDrawer *physics_debug_drawer;
};


static void PhysicsDebug_free(LIExtPhysicsDebug* self)
{
	/*Remove the debug drawer from physics simulation.*/
	if(self->physics != NULL && self->physics->dynamics->getDebugDrawer() == self->physics_debug_drawer) {
		self->physics->dynamics->setDebugDrawer(NULL);
	}
	/*Delete debug drawer*/
	if(self->physics_debug_drawer != NULL) {
		delete self->physics_debug_drawer;
	}

	lisys_free(self);
}

static void PhysicsDebug_new(LIScrArgs* args)
{
	LIExtPhysicsDebugModule* module;
	LIExtPhysicsDebug *self;
	LIScrData* data;
	LIPhyPhysics *physics;
	LIRenRender *render;

	module = (LIExtPhysicsDebugModule *)liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_DEBUG);
	if(module == NULL) {
		return;
	}
	render = (LIRenRender *)limai_program_find_component(module->program, "render");
	if(render == NULL) {
		return;
	}
	physics = (LIPhyPhysics *)limai_program_find_component(module->program, "physics");
	if(physics == NULL) {
		return;
	}

	/*Allocate self.*/
	self = (LIExtPhysicsDebug *)lisys_calloc (1, sizeof (LIExtPhysicsDebug));
	if(self == NULL) {
		return;
	}
	self->module = module;
	self->render = render;
	self->physics = physics;

	/*Create the debug drawer.*/
	self->physics_debug_drawer = new OgreDebugDrawer(render->scene_manager);
	if(self->physics_debug_drawer == NULL) {
		PhysicsDebug_free(self);
		return;
	}

	/* Allocate userdata. */
	data = (LIScrData *)liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_PHYSICS_DEBUG, (LIScrGCFunc)PhysicsDebug_free);
	if(data == NULL) {
		PhysicsDebug_free(self);
		return;
	}

	self->physics_debug_drawer = new OgreDebugDrawer(render->scene_manager);
	self->physics_debug_drawer->setDebugMode( btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawContactPoints );
	self->physics->dynamics->setDebugDrawer(self->physics_debug_drawer);
	liscr_args_seti_stack (args);
}


static void PhysicsDebug_draw(LIScrArgs* args)
{
	LIExtPhysicsDebug *self = (LIExtPhysicsDebug *)args->self;
	self->physics->dynamics->debugDrawWorld();
}

void liext_script_physics_debug (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_PHYSICS_DEBUG, "physics_debug_new", PhysicsDebug_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_DEBUG, "physics_debug_draw", PhysicsDebug_draw);
}

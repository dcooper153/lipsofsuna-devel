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
 * \addtogroup liext Extension
 * @{
 * \addtogroup LIExtSpeech Speech
 * @{
 */

#include "ext-module.h"
#include "ext-speech.h"

/* @luadoc
 * module "Extension.Speech"
 * --- Display speech above objects.
 * -- @name Sound
 * -- @class table
 */

/* @luadoc
 * --- Displays a message above the object.
 * -- @param self Speech class.
 * -- @param args Arguments.<ul>
 * --   <li>diffuse: Diffuse color.</li>
 * --   <li>fade_exponent: Fade exponent.</li>
 * --   <li>fade_time: Fade time in seconds, or nil.</li>
 * --   <li>life_time: Life time in seconds, or nil.</li>
 * --   <li>font: Font name or nil.</li>
 * --   <li>object: Object.</li>
 * --   <li>message: Speech string. (required)</li></ul>
 * function Speech.add(self, args)
 */
static void Speech_add (LIScrArgs* args)
{
	int id;
	float diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const char* msg;
	const char* font = "default";
	LIScrData* object;
	LIExtModule* module;
	LIExtSpeech* speech;

	if (!liscr_args_gets_string (args, "message", &msg))
		return;
	liscr_args_gets_floatv (args, "diffuse", 4, diffuse);
	liscr_args_gets_string (args, "font", &font);
	if (liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &object))
		id = ((LIEngObject*) object->data)->id;
	else
		return;
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SPEECH);
	speech = liext_speeches_set_speech (module, id, diffuse, font, msg);
	if (speech != NULL)
	{
		if (liscr_args_gets_float (args, "fade_exponent", &speech->fade_exponent))
			speech->fade_exponent = LIMAT_MAX (1.0f, speech->fade_exponent);
		if (liscr_args_gets_float (args, "fade_time", &speech->fade_time))
			speech->fade_time = LIMAT_MAX (0.0f, speech->fade_time);
		if (liscr_args_gets_float (args, "life_time", &speech->life_time))
			speech->life_time = LIMAT_MAX (0.0f, speech->life_time);
	}
}

/* @luadoc
 * --- Draws the speech texts to the framebuffer.
 * -- @param self Speech class.
 * -- @param args Arguments.<ul>
 * --   <li>modelview: Modelview matrix.</li>
 * --   <li>projection: Projection matrix.</li>
 * --   <li>viewport: Viewport array.</li></ul>
 * function Speech.draw(self)
 */
static void Speech_draw (LIScrArgs* args)
{
	GLint viewport[4];
	LIExtModule* module;
	LIMatMatrix modelview;
	LIMatMatrix projection;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SPEECH);

	/* Get arguments. */
	modelview = limat_matrix_identity ();
	projection = limat_matrix_identity ();
	liscr_args_gets_floatv (args, "modelview", 16, modelview.m);
	liscr_args_gets_floatv (args, "projection", 16, projection.m);
	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = module->client->window->mode.width;
	viewport[3] = module->client->window->mode.height;
	liscr_args_gets_intv (args, "viewport", 4, viewport);
	viewport[0] = LIMAT_MAX (0, viewport[0]);
	viewport[1] = LIMAT_MAX (0, viewport[1]);
	viewport[1] = module->client->window->mode.height - viewport[1] - viewport[3];
	viewport[2] = LIMAT_MAX (2, viewport[2]);
	viewport[3] = LIMAT_MAX (2, viewport[3]);

	/* Render all speech. */
	liext_speeches_render (module, &projection, &modelview, viewport);
}

/*****************************************************************************/

void
liext_script_speech (LIScrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SPEECH, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "add", Speech_add);
	liscr_class_insert_cfunc (self, "draw", Speech_draw);
}

/** @} */
/** @} */

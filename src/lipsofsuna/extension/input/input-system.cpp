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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIInpInput Input
 * @{
 */

#include "input.hpp"
#include "input-system.hpp"

LIInpSystem::LIInpSystem (LIInpInput* input, bool grab) :
	grab (grab),
	input (input),
	input_manager (NULL),
	keyboard (NULL),
	mouse (NULL)
{
	/* Enable or disable mouse input grabs. */
	OIS::ParamList params;
	if (!grab)
	{
#if defined OIS_WIN32_PLATFORM
		// FIXME: This causes device initialization to throw an error.
		//params.insert (std::make_pair(std::string ("w32_mouse"), std::string ("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
		params.insert (std::make_pair (std::string ("x11_mouse_grab"), std::string ("false")));
#endif
	}

	/* Always disable keyboard grabs so that you can close the window with Alt+F4. */
#if defined OIS_WIN32_PLATFORM
	// FIXME: This causes device initialization to throw an error.
	//params.insert (std::make_pair(std::string ("w32_keyboard"), std::string ("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
	params.insert (std::make_pair (std::string ("x11_keyboard_grab"), std::string ("false")));
	params.insert (std::make_pair (std::string ("XAutoRepeatOn"), std::string ("true")));
#endif

	/* Get the window handle. */
	size_t window = 0;
	input->render->data->render_window->getCustomAttribute ("WINDOW", &window);
	params.insert (std::make_pair (std::string("WINDOW"), Ogre::StringConverter::toString (window)));

	/* Initialize the input manager. */
	this->input_manager = OIS::InputManager::createInputSystem (params);

	/* Initialize mouse. */
	try
	{
		int left, top;
		unsigned int width, height, depth;
		this->mouse = (OIS::Mouse*) this->input_manager->createInputObject (OIS::OISMouse, true);
		this->mouse->setEventCallback (this);
		this->input->render->data->render_window->getMetrics ( width, height, depth, left, top );
		const OIS::MouseState &mouse_state = this->mouse->getMouseState ();
		mouse_state.width  = width;
		mouse_state.height = height;
	}
	catch (...)
	{
	}

	/* Initialize keyboard. */
	try
	{
		this->keyboard = (OIS::Keyboard*) this->input_manager->createInputObject (OIS::OISKeyboard, true);
		this->keyboard->setEventCallback (this);
		this->keyboard->setTextTranslation (OIS::Keyboard::Unicode);
	}
	catch (...)
	{
	}
}

LIInpSystem::~LIInpSystem ()
{
	/* Uninitialize devices. */
	if (this->mouse != NULL)
		this->input_manager->destroyInputObject (this->mouse);
	if (this->keyboard != NULL)
		this->input_manager->destroyInputObject (this->keyboard);

	/* Uninitialize the input manager. */
	if (this->input_manager != NULL)
		OIS::InputManager::destroyInputSystem (this->input_manager);
}

int LIInpSystem::get_mouse_button_state ()
{
	const OIS::MouseState& state = this->mouse->getMouseState ();
	return state.buttons;
}

void LIInpSystem::get_mouse_state (int* x, int* y)
{
	const OIS::MouseState& state = this->mouse->getMouseState ();
	*x = state.X.abs;
	*y = state.Y.abs;
}

void LIInpSystem::update (float secs)
{
	if (this->keyboard != NULL)
		this->keyboard->capture ();
	if (this->mouse != NULL)
		this->mouse->capture ();
}

bool LIInpSystem::keyPressed (const OIS::KeyEvent& event)
{
	if (event.key == OIS::KC_F4 && this->keyboard->isModifierDown (OIS::Keyboard::Alt))
		this->input->program->quit = 1;
	this->emit_key_event (event, true);
	return true;
}

bool LIInpSystem::keyReleased (const OIS::KeyEvent& event)
{
	this->emit_key_event (event, false);
	return true;
}

bool LIInpSystem::mouseMoved (const OIS::MouseEvent& event)
{
	if (event.state.X.rel || event.state.Y.rel)
	{
		limai_program_event (this->input->program, "mousemotion", "x", LISCR_TYPE_INT, event.state.X.abs, "y", LISCR_TYPE_INT, event.state.Y.abs, "dx", LISCR_TYPE_INT, event.state.X.rel, "dy", LISCR_TYPE_INT, event.state.Y.rel, NULL);
	}
	if (event.state.Z.rel)
	{
		limai_program_event (this->input->program, "mousescroll", "abs", LISCR_TYPE_INT, event.state.Z.abs, "rel", LISCR_TYPE_INT, event.state.Z.rel, NULL);
	}
	return true;
}

bool LIInpSystem::mousePressed (const OIS::MouseEvent& event, OIS::MouseButtonID id)
{
	limai_program_event (this->input->program, "mousepress", "button", LISCR_TYPE_INT, translate_mouse_button (id), "x", LISCR_TYPE_INT, event.state.X.abs, "y", LISCR_TYPE_INT, event.state.Y.abs, NULL);
	return true;
}

bool LIInpSystem::mouseReleased (const OIS::MouseEvent& event, OIS::MouseButtonID id)
{
	limai_program_event (this->input->program, "mouserelease", "button", LISCR_TYPE_INT, translate_mouse_button (id), "x", LISCR_TYPE_INT, event.state.X.abs, "y", LISCR_TYPE_INT, event.state.Y.abs, NULL);
	return true;
}

void LIInpSystem::emit_key_event (const OIS::KeyEvent& event, bool press)
{
	int mods = 0;
	char* str = NULL;

	if (this->keyboard->isKeyDown (OIS::KC_LSHIFT)) mods |= 0x0001;
	if (this->keyboard->isKeyDown (OIS::KC_RSHIFT)) mods |= 0x0002;
	if (this->keyboard->isKeyDown (OIS::KC_LCONTROL)) mods |= 0x0040;
	if (this->keyboard->isKeyDown (OIS::KC_RCONTROL)) mods |= 0x0080;
	if (this->keyboard->isKeyDown (OIS::KC_LMENU)) mods |= 0x0100;
	if (this->keyboard->isKeyDown (OIS::KC_RMENU)) mods |= 0x0200;
	if (this->keyboard->isKeyDown (OIS::KC_LWIN)) mods |= 0x0400;
	if (this->keyboard->isKeyDown (OIS::KC_RWIN)) mods |= 0x0800;
	//if (this->keyboard->isKeyDown (OIS::KC_NUMLOCK)) mods |= 0x1000;
	//if (this->keyboard->isKeyDown (OIS::KC_CAPSLOCK)) mods |= 0x2000;
	//if (this->keyboard->isKeyDown (OIS::KC_MODE)) mods |= 0x4000;

	if (event.text != 0)
		str = lisys_wchar_to_utf8 (event.text);
	if (str != NULL)
	{
		limai_program_event (this->input->program, press? "keypress" : "keyrelease", "code", LISCR_TYPE_INT, event.key, "mods", LISCR_TYPE_INT, mods, "text", LISCR_TYPE_STRING, str, NULL);
		lisys_free (str);
	}
	else
	{
		limai_program_event (this->input->program, press? "keypress" : "keyrelease", "code", LISCR_TYPE_INT, event.key, "mods", LISCR_TYPE_INT, mods, NULL);
	}
}

int LIInpSystem::translate_mouse_button (OIS::MouseButtonID id)
{
	switch (id)
	{
		case OIS::MB_Left: return 1;
		case OIS::MB_Middle: return 2;
		case OIS::MB_Right: return 3;
		case OIS::MB_Button3: return 4;
		case OIS::MB_Button4: return 5;
		case OIS::MB_Button5: return 6;
		case OIS::MB_Button6: return 7;
		case OIS::MB_Button7: return 8;
		default:
			return 0;
	}
}

/* TODO: Joystick.
	case SDL_JOYAXISMOTION:
		limai_program_event (self->program, "joystickmotion", "axis", LISCR_TYPE_INT, event.jaxis.axis + 1, "value", LISCR_TYPE_FLOAT, event.jaxis.value / 32768.0f, NULL);
		return 0;
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		limai_program_event (self->program, (event.type == SDL_JOYBUTTONDOWN)? "joystickpress" : "joystickrelease", "button", LISCR_TYPE_INT, event.jbutton.button, NULL);
		return 0;
*/

/* TODO: Low resource consumption when inactive.
	case SDL_ACTIVEEVENT:
		if (event.active.state & SDL_APPINPUTFOCUS)
		{
			if (event.active.gain)
			{
				self->active = 1;
				self->program->sleep = 0;
				if (self->grab)
					private_grab (self, 1);
			}
			else
			{
				self->active = 0;
				self->program->sleep = 100000;
				if (self->grab)
					private_grab (self, 0);
			}
		}
		break;
*/

/* TODO: Synchronize mouse rectangle with the video mode.
	case SDL_VIDEORESIZE:
		self->mode.width = event.resize.w;
		self->mode.height = event.resize.h;
		break;
*/

/** @} */
/** @} */

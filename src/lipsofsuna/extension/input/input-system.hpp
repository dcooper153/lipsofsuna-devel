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

#ifndef __EXT_INPUT_INPUT_SYSTEM_H__
#define __EXT_INPUT_INPUT_SYSTEM_H__

#include "input.h"
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class LIInpSystem : public OIS::KeyListener, public OIS::MouseListener
{
public:
	LIInpSystem (LIInpInput* input, bool grab);
	virtual ~LIInpSystem ();
	void get_mouse_state (int* x, int* y);
	void update (float secs);
protected:
	virtual bool keyPressed (const OIS::KeyEvent& event);
	virtual bool keyReleased (const OIS::KeyEvent& event);
	virtual bool mouseMoved (const OIS::MouseEvent& event);
	virtual bool mousePressed (const OIS::MouseEvent& event, OIS::MouseButtonID id);
	virtual bool mouseReleased (const OIS::MouseEvent& event, OIS::MouseButtonID id);
	void emit_key_event (const OIS::KeyEvent& event, bool press);
	int translate_mouse_button (OIS::MouseButtonID id);
public:
	bool grab;
	LIInpInput* input;
	OIS::InputManager* input_manager;
	OIS::Keyboard* keyboard;
	OIS::Mouse* mouse;
};

#endif

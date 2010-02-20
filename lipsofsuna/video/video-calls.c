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
 * \addtogroup livid Video
 * @{
 * \addtogroup LIVidCalls Calls
 * @{
 */

#include <lipsofsuna/system.h>
#include "video-calls.h"

int
livid_calls_init (LIVidCalls* self)
{
	/* SDL calls. */
#define CALL(name) self->name = name;
	CALL (SDL_ConvertSurface);
	CALL (SDL_Delay);
	CALL (SDL_EnableKeyRepeat);
	CALL (SDL_EnableUNICODE);
	CALL (SDL_FreeSurface);
	CALL (SDL_GetModState);
	CALL (SDL_GetMouseState);
	CALL (SDL_GetTicks);
	CALL (SDL_GetVideoSurface);
	CALL (SDL_GL_SetAttribute);
	CALL (SDL_GL_SwapBuffers);
	CALL (SDL_WM_GrabInput);
	CALL (SDL_Init);
	CALL (SDL_JoystickClose);
	CALL (SDL_JoystickOpen);
	CALL (SDL_PollEvent);
	CALL (SDL_Quit);
	CALL (SDL_SetVideoMode);
	CALL (SDL_ShowCursor);
	CALL (SDL_WarpMouse);
	CALL (SDL_WM_SetCaption);
#undef CALL

	/* SDL_ttf calls. */
#define CALL(name) self->name = name;
	CALL (TTF_CloseFont);
	CALL (TTF_FontLineSkip);
	CALL (TTF_FontAscent);
	CALL (TTF_FontDescent);
	CALL (TTF_FontHeight);
	CALL (TTF_GlyphMetrics);
	CALL (TTF_Init);
	CALL (TTF_OpenFont);
	CALL (TTF_Quit);
	CALL (TTF_RenderGlyph_Blended);
	CALL (TTF_WasInit);
#undef CALL

	return 1;
}

/** @} */
/** @} */

/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup lividCalls Calls
 * @{
 */

#ifndef __VIDEO_CALLS_H__
#define __VIDEO_CALLS_H__

#include <SDL.h>
#include <SDL_main.h>
#include <SDL_ttf.h>
#include <system/lips-system.h>

typedef struct _lividCalls lividCalls;
struct _lividCalls
{
	lisysModule* libSDL;
	lisysModule* libSDL_ttf;
	SDL_Surface* (*SDL_ConvertSurface)(SDL_Surface *src, SDL_PixelFormat *fmt, Uint32 flags);
	void (*SDL_Delay)(Uint32 ms);
	int (*SDL_EnableKeyRepeat)(int delay, int interval);
	int (*SDL_EnableUNICODE)(int enable);
	void (*SDL_FreeSurface)(SDL_Surface *surface);
	SDLMod (*SDL_GetModState)(void);
	Uint8 (*SDL_GetMouseState)(int *x, int *y);
	Uint32 (*SDL_GetTicks)();
	SDL_Surface* (*SDL_GetVideoSurface)();
	int (*SDL_GL_SetAttribute)(SDL_GLattr attr, int value);
	void (*SDL_GL_SwapBuffers)(void );
	SDL_GrabMode (*SDL_WM_GrabInput)(SDL_GrabMode mode);
	int (*SDL_Init)(Uint32 flags);
	void (*SDL_JoystickClose)(SDL_Joystick *joystick);
	SDL_Joystick* (*SDL_JoystickOpen)(int index);
	int (*SDL_PollEvent)(SDL_Event *event);
	void (*SDL_Quit)(void);
	SDL_Surface* (*SDL_SetVideoMode)(int width, int height, int bpp, Uint32 flags);
	int (*SDL_ShowCursor)(int toggle);
	void (*SDL_WarpMouse)(Uint16 x, Uint16 y);
	void (*SDL_WM_SetCaption)(const char *title, const char *icon);
	void (*TTF_CloseFont)(TTF_Font *font);
	int (*TTF_FontLineSkip)(const TTF_Font *font);
	int (*TTF_FontAscent)(const TTF_Font *font);
	int (*TTF_FontDescent)(const TTF_Font *font);
	int (*TTF_FontHeight)(const TTF_Font *font);
	int (*TTF_GlyphMetrics)(TTF_Font *font, Uint16 ch, int *minx, int *maxx, int *miny, int *maxy, int *advance);
	int (*TTF_Init)(void);
	TTF_Font* (*TTF_OpenFont)(const char *file, int ptsize);
	void (*TTF_Quit)(void);
	SDL_Surface* (*TTF_RenderGlyph_Blended)(TTF_Font *font, Uint16 ch, SDL_Color fg);
	int (*TTF_WasInit)(void);
};

int
livid_calls_init (lividCalls* self);

#endif

/** @} */
/** @} */

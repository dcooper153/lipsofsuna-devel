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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndDraw Draw
 * @{
 */

#ifndef __RENDER_DRAW_H__
#define __RENDER_DRAW_H__

#include "render.h"
#include "render-object.h"
#include "render-types.h"

void
lirnd_draw_bounds (lirndContext* context,
                   lirndObject*  object,
                   void*         data);

void
lirnd_draw_debug (lirndContext* context,
                  lirndObject*  object,
                  void*         data);

void
lirnd_draw_exclude (lirndContext* context,
                    lirndObject*  object,
                    void*         data);

void
lirnd_draw_opaque (lirndContext* context,
                   lirndObject*  object,
                   void*         data);

void
lirnd_draw_picking (lirndContext* context,
                    lirndObject*  object,
                    void*         data);

void
lirnd_draw_shadeless (lirndContext* context,
                      lirndObject*  object,
                      void*         data);

void
lirnd_draw_shadowmap (lirndContext* context,
                      lirndObject*  object,
                      void*         data);

void
lirnd_draw_transparent (lirndContext* context,
                        lirndObject*  object,
                        void*         data);

#endif

/** @} */
/** @} */

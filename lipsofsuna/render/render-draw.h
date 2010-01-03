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
 * \addtogroup liren Render
 * @{
 * \addtogroup lirenDraw Draw
 * @{
 */

#ifndef __RENDER_DRAW_H__
#define __RENDER_DRAW_H__

#include "render.h"
#include "render-object.h"
#include "render-types.h"

void
liren_draw_bounds (LIRenContext* context,
                   LIRenObject*  object,
                   void*         data);

void
liren_draw_debug (LIRenContext* context,
                  LIRenObject*  object,
                  void*         data);

void
liren_draw_exclude (LIRenContext* context,
                    LIRenObject*  object,
                    void*         data);

void
liren_draw_hair (LIRenContext* context,
                 LIRenObject*  object,
                 void*         data);

void
liren_draw_opaque (LIRenContext* context,
                   LIRenObject*  object,
                   void*         data);

void
liren_draw_picking (LIRenContext* context,
                    LIRenObject*  object,
                    void*         data);

void
liren_draw_shadeless (LIRenContext* context,
                      LIRenObject*  object,
                      void*         data);

void
liren_draw_shadowmap (LIRenContext* context,
                      LIRenObject*  object,
                      void*         data);

void
liren_draw_transparent (LIRenContext* context,
                        LIRenObject*  object,
                        void*         data);

#endif

/** @} */
/** @} */

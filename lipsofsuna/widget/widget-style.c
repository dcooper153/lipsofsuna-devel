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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup LIWdgStyle Style
 * @{
 */

#include <lipsofsuna/font.h>
#include <lipsofsuna/image.h>
#include "widget-manager.h"
#include "widget-style.h"

static void
private_paint_scaled (LIWdgStyle* self,
                      LIWdgRect*  rect);

static void
private_paint_tiled (LIWdgStyle* self,
                     LIWdgRect*  rect);

/*****************************************************************************/

/**
 * \brief Paints widget graphics.
 *
 * \param self Style.
 * \param rect Rectangle.
 */
void
liwdg_style_paint (LIWdgStyle* self,
                   LIWdgRect*  rect)
{
	if (self->texture == NULL)
		return;
	if (self->scale)
		private_paint_scaled (self, rect);
	else
		private_paint_tiled (self, rect);
}

/*****************************************************************************/

static void
private_paint_scaled (LIWdgStyle* self,
                      LIWdgRect*  rect)
{
	int px;
	int py;
	int pw;
	int ph;
	float center;
	float size;
	float xs;
	float ys;
	float tx[2];
	float ty[2];

	/* Calculate texture coordinates. */
	tx[0] = (float)(self->x) / self->texture->width;
	tx[1] = (float)(self->x + self->w[1]) / self->texture->width;
	ty[0] = (float)(self->y) / self->texture->height;
	ty[1] = (float)(self->y + self->h[1]) / self->texture->height;

	/* Calculate pixels per texture unit. */
	xs = tx[1] - tx[0];
	ys = ty[1] - ty[0];
	if (xs < LIMAT_EPSILON || ys < LIMAT_EPSILON)
		return;
	xs = rect->width / xs;
	ys = rect->height / ys;

	/* Scale and translate to fill widget area. */
	if (ty[1] - ty[0] >= rect->height / xs)
	{
		center = 0.5f * (ty[0] + ty[1]);
		size = ty[1] - ty[0];
		ty[0] = center - 0.5f * rect->height / xs;
		ty[1] = center + 0.5f * rect->height / xs;
	}
	else
	{
		center = 0.5f * (tx[0] + tx[1]);
		size = tx[1] - tx[0];
		tx[0] = center - 0.5f * rect->width / ys;
		tx[1] = center + 0.5f * rect->width / ys;
	}

	/* Bind texture. */
	glBindTexture (GL_TEXTURE_2D, self->texture->texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor3f (1.0f, 1.0f, 1.0f);

	/* Paint fill. */
	px = rect->x;
	py = rect->y;
	pw = rect->width;
	ph = rect->height;
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tx[0], ty[0]); glVertex2f (px     , py);
	glTexCoord2f (tx[1], ty[0]); glVertex2f (px + pw, py);
	glTexCoord2f (tx[0], ty[1]); glVertex2f (px     , py + ph);
	glTexCoord2f (tx[1], ty[1]); glVertex2f (px + pw, py + ph);
	glEnd ();
}

static void
private_paint_tiled (LIWdgStyle* self,
                     LIWdgRect*  rect)
{
	int px;
	int py;
	float fw;
	float fh;
	float fu;
	float fv;
	float w[3];
	float h[3];
	float tx[4];
	float ty[4];

	/* Calculate repeat counts. */
	w[0] = self->w[0];
	w[1] = LIMAT_MAX (1, self->w[1]);
	w[2] = self->w[2];
	h[0] = self->h[0];
	h[1] = LIMAT_MAX (1, self->h[1]);
	h[2] = self->h[2];

	/* Calculate texture coordinates. */
	tx[0] = (float)(self->x) / self->texture->width;
	tx[1] = (float)(self->x + self->w[0]) / self->texture->width;
	tx[2] = (float)(self->x + self->w[0] + self->w[1]) / self->texture->width;
	tx[3] = (float)(self->x + self->w[0] + self->w[1] + self->w[2]) / self->texture->width;
	ty[0] = (float)(self->y) / self->texture->height;
	ty[1] = (float)(self->y + self->h[0]) / self->texture->height;
	ty[2] = (float)(self->y + self->h[0] + self->h[1]) / self->texture->height;
	ty[3] = (float)(self->y + self->h[0] + self->h[1] + self->h[2]) / self->texture->height;

	/* Bind texture. */
	glBindTexture (GL_TEXTURE_2D, self->texture->texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glColor3f (1.0f, 1.0f, 1.0f);

	/* Paint corners. */
	px = rect->x;
	py = rect->y;
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tx[0], ty[0]); glVertex2f (px       , py);
	glTexCoord2f (tx[1], ty[0]); glVertex2f (px + w[0], py);
	glTexCoord2f (tx[0], ty[1]); glVertex2f (px       , py + h[0]);
	glTexCoord2f (tx[1], ty[1]); glVertex2f (px + w[0], py + h[0]);
	glEnd ();
	px = rect->x + rect->width - w[2] - 1;
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tx[2], ty[0]); glVertex2f (px       , py);
	glTexCoord2f (tx[3], ty[0]); glVertex2f (px + w[2], py);
	glTexCoord2f (tx[2], ty[1]); glVertex2f (px       , py + h[0]);
	glTexCoord2f (tx[3], ty[1]); glVertex2f (px + w[2], py + h[0]);
	glEnd ();
	py = rect->y + rect->height - h[2] - 1;
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tx[2], ty[2]); glVertex2f (px       , py);
	glTexCoord2f (tx[3], ty[2]); glVertex2f (px + w[2], py);
	glTexCoord2f (tx[2], ty[3]); glVertex2f (px       , py + h[2]);
	glTexCoord2f (tx[3], ty[3]); glVertex2f (px + w[2], py + h[2]);
	glEnd ();
	px = rect->x;
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tx[0], ty[2]); glVertex2f (px       , py);
	glTexCoord2f (tx[1], ty[2]); glVertex2f (px + w[0], py);
	glTexCoord2f (tx[0], ty[3]); glVertex2f (px       , py + h[2]);
	glTexCoord2f (tx[1], ty[3]); glVertex2f (px + w[0], py + h[2]);
	glEnd ();

	/* Paint horizontal borders. */
	for (px = rect->x + w[0] ; px < rect->x + rect->width - w[2] ; px += w[1])
	{
		fw = LIMAT_MIN (w[1], rect->x + rect->width - px - w[2] - 1);
		fu = tx[1] + (tx[2] - tx[1]) * fw / w[1];
		py = rect->y;
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (tx[1], ty[0]); glVertex2f (px     , py);
		glTexCoord2f (fu   , ty[0]); glVertex2f (px + fw, py);
		glTexCoord2f (tx[1], ty[1]); glVertex2f (px     , py + h[0]);
		glTexCoord2f (fu   , ty[1]); glVertex2f (px + fw, py + h[0]);
		glEnd ();
		py = rect->y + rect->height - h[2] - 1;
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (tx[1], ty[2]); glVertex2f (px     , py);
		glTexCoord2f (fu   , ty[2]); glVertex2f (px + fw, py);
		glTexCoord2f (tx[1], ty[3]); glVertex2f (px     , py + h[2]);
		glTexCoord2f (fu   , ty[3]); glVertex2f (px + fw, py + h[2]);
		glEnd ();
	}

	/* Paint vertical borders. */
	for (py = rect->y + h[0] ; py < rect->y + rect->height - h[2] ; py += h[1])
	{
		fh = LIMAT_MIN (h[1], rect->y + rect->height - py - h[2] - 1);
		fv = ty[1] + (ty[2] - ty[1]) * fh / h[1];
		px = rect->x;
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (tx[0], ty[1]); glVertex2f (px       , py);
		glTexCoord2f (tx[1], ty[1]); glVertex2f (px + w[0], py);
		glTexCoord2f (tx[0], fv   ); glVertex2f (px       , py + fh);
		glTexCoord2f (tx[1], fv   ); glVertex2f (px + w[0], py + fh);
		glEnd ();
		px = rect->x + rect->width - w[2] - 1;
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (tx[2], ty[1]); glVertex2f (px       , py);
		glTexCoord2f (tx[3], ty[1]); glVertex2f (px + w[2], py);
		glTexCoord2f (tx[2], fv   ); glVertex2f (px       , py + fh);
		glTexCoord2f (tx[3], fv   ); glVertex2f (px + w[2], py + fh);
		glEnd ();
	}

	/* Paint fill. */
	for (py = rect->y + h[0] ; py < rect->y + rect->height - h[2] ; py += h[1])
	for (px = rect->x + w[0] ; px < rect->x + rect->width - w[2] ; px += w[1])
	{
		fw = LIMAT_MIN (w[1], rect->x + rect->width - px - w[2] - 1);
		fh = LIMAT_MIN (h[1], rect->y + rect->height - py - h[2] - 1);
		fu = tx[1] + (tx[2] - tx[1]) * fw / w[1];
		fv = ty[1] + (ty[2] - ty[1]) * fh / h[1];
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (tx[1], ty[1]); glVertex2f (px     , py);
		glTexCoord2f (fu   , ty[1]); glVertex2f (px + fw, py);
		glTexCoord2f (tx[1], fv   ); glVertex2f (px     , py + fh);
		glTexCoord2f (fu   , fv   ); glVertex2f (px + fw, py + fh);
		glEnd ();
	}
}

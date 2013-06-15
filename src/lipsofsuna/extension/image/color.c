#include "lipsofsuna/math.h"
#include "color.h"

#define EPSILON 0.000001

/**
 * \brief Converts an RGB color to HSV.
 * \param self RGB color.
 * \param result Return location for the HSV color.
 */
void liimg_color_rgb_to_hsv (
	const LIImgColor* self,
	LIImgColorHSV*    result)
{
	float r, g, b, v, m, c, h, s;

	r = self->r / 255.0f;
	g = self->g / 255.0f;
	b = self->b / 255.0f;
	v = LIMAT_MAX (LIMAT_MAX (r, g), b);
	m = LIMAT_MIN (LIMAT_MIN (r, g), b);
	c = v - m;
	if (c < EPSILON)
		h = 0;
	else if (v == r)
		h = fmodf ((g - b) / c, 6.0f) / 6.0f;
	else if (v == g)
		h = ((b - r) / c + 2.0f) / 6.0f;
	else
		h = ((r - g) / c + 4.0f) / 6.0f;
	if (c < EPSILON)
		s = 0;
	else
		s = c / v;
	result->h = h;
	result->s = s;
	result->v = v;
}

/**
 * \brief Converts an HSV color to RGB.
 * \param self HSV color.
 * \param result Return location for the RGB color.
 */
LIAPICALL (void, liimg_color_hsv_to_rgb, (
	const LIImgColorHSV* self,
	LIImgColor*          result))
{
	float c, l, hh, x, r, g, b;

	c = self->v * self->s;
	l = self->v - c;
	hh = self->h * 6.0f;
	x = c * (1.0f - fabsf (fmodf (hh, 2.0f) - 1.0f));
	if (0.0f <= hh && hh < 1.0f)
	{
		r = c + l;
		g = x + l;
		b = l;
	}
	else if (1.0f <= hh && hh < 2.0f)
	{
		r = x + l;
		g = c + l;
		b = l;
	}
	else if (2.0f <= hh && hh < 3.0f)
	{
		r = l;
		g = c + l;
		b = x + l;
	}
	else if (3.0f <= hh && hh < 4.0f)
	{
		r = l;
		g = x + l;
		b = c + l;
	}
	else if (4.0f <= hh && hh < 5.0f)
	{
		r = x + l;
		g = l;
		b = c + l;
	}
	else
	{
		r = c + l;
		g = l;
		b = x + l;
	}
	result->r = (uint8_t)(r * 255.0f + 0.5f);
	result->g = (uint8_t)(g * 255.0f + 0.5f);
	result->b = (uint8_t)(b * 255.0f + 0.5f);
}

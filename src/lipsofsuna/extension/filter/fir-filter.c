/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
 * \addtogroup LIExtFilter Filter
 * @{
 */

#include "fir-filter.h"

/**
 * \brief Creates a new finite impulse response filter.
 * \param length Delay line length.
 * \param coeffs Filter coeffiecients.
 * \return New FIR filter.
 */
LIExtFirFilter* liext_fir_filter_new (
	int          length,
	const float* coeffs)
{
	LIExtFirFilter* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtFirFilter));
	if (self == NULL)
		return NULL;
	self->offset = 1;

	/* Allocate the coefficients. */
	self->coeffs = lisys_calloc (length, sizeof (float));
	if (self->coeffs == NULL)
	{
		liext_fir_filter_free (self);
		return NULL;
	}

	/* Allocate the delay line. */
	self->delay_line = lisys_calloc (length, sizeof (float));
	if (self->delay_line == NULL)
	{
		liext_fir_filter_free (self);
		return NULL;
	}

	/* Copy the coefficients. */
	self->length = length;
	memcpy(self->coeffs, coeffs, length * sizeof (float));

	return self;
}

/**
 * \brief Frees the FIR filter.
 * \param self FIR filter.
 */
void liext_fir_filter_free (
	LIExtFirFilter* self)
{
	lisys_free (self->coeffs);
	lisys_free (self->delay_line);
	lisys_free (self);
}

/**
 * \brief Pushes a sample into the delay line.
 * \param self FIR filter.
 * \param sample Sample.
 */
void liext_fir_filter_add (
	LIExtFirFilter* self,
	float           sample)
{
	--self->offset;
	if (self->offset < 0)
		self->offset = self->length - 1;
	self->delay_line[self->offset] = sample;
	self->dirty = 1;
}

/**
 * \brief Gets the filter output.
 * \param self FIR filter.
 * \return Filter output.
 */
float liext_fir_filter_get (
	LIExtFirFilter* self)
{
	int i;
	int coeff;

	if (self->dirty)
	{
		coeff = 0;
		self->output = 0.0f;
		for (i = self->offset ; i < self->length ; i++, coeff++)
			self->output += self->coeffs[coeff] * self->delay_line[i];
		for (i = 0 ; coeff < self->length ; i++, coeff++)
			self->output += self->coeffs[coeff] * self->delay_line[i];
	}

	return self->output;
}

/** @} */
/** @} */

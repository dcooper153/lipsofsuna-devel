/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlIpo Ipo
 * @{
 */

#include <stdio.h>
#include <assert.h>
#include "model-ipo.h"

int
limdl_ipo_read (limdlIpo* self,
                liReader* reader)
{
	int i;
	int j;
	int channels;
	uint32_t tmp[2];

	/* Read the number of channels. */
	if (!li_reader_get_text (reader, "", &self->name) ||
	    !li_reader_get_uint32 (reader, tmp))
		return 0;
	channels = tmp[0];
	if (channels != LIMDL_IPO_CHANNEL_NUM)
	{
		lisys_error_set (EINVAL, "invalid ipo channel");
		return 0;
	}

	/* Read each channel. */
	for (i = 0 ; i < channels ; i++)
	{
		/* Read the header. */
		if (!li_reader_get_uint32 (reader, tmp + 0) ||
		    !li_reader_get_uint32 (reader, tmp + 1))
			return 0;
		self->channels[i].type = tmp[0];
		self->channels[i].length = tmp[1];

		if (self->channels[i].length)
		{
			/* Allocate memory. */
			self->channels[i].nodes = calloc (self->channels[i].length, sizeof (float));
			if (self->channels[i].nodes == NULL)
			{
				lisys_error_set (ENOMEM, NULL);
				return 0;
			}

			/* Read the data. */
			for (j = 0 ; j < self->channels[i].length ; j++)
			{
				if (!li_reader_get_float (reader, self->channels[i].nodes + j))
					return 0;
			}
		}
	}

	return 1;
}

int
limdl_ipo_write (const limdlIpo* self,
                 liarcWriter*    writer)
{
	uint32_t i;
	uint32_t j;

	if (!liarc_writer_append_string (writer, self->name) ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, LIMDL_IPO_CHANNEL_NUM))
		return 0;
	for (i = 0 ; i < LIMDL_IPO_CHANNEL_NUM ; i++)
	{
		if (!liarc_writer_append_uint32 (writer, self->channels[i].type) ||
		    !liarc_writer_append_uint32 (writer, self->channels[i].length))
			return 0;
		for (j = 0 ; j < self->channels[i].length ; j++)
		{
			if (!liarc_writer_append_float (writer, self->channels[i].nodes[j]))
				return 0;
		}
	}

	return 1;
}

float
limdl_ipo_get_duration (const limdlIpo* self)
{
	int i;
	float tmp;
	float duration = 0.0f;

	for (i = 0 ; i < LIMDL_IPO_CHANNEL_NUM ; i++)
	{
		tmp = limdl_ipo_channel_get_duration (self->channels + i);
		if (duration < tmp)
			duration = tmp;
	}

	return duration;
}

void
limdl_ipo_get_location (const limdlIpo* self,
                        float           time,
                        limatVector*    vector)
{
	vector->x = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_LOCX, time);
	vector->y = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_LOCY, time);
	vector->z = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_LOCZ, time);
}

void
limdl_ipo_get_quaternion (const limdlIpo*  self,
                          float            time,
                          limatQuaternion* quaternion)
{
	quaternion->x = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_QUATX, time);
	quaternion->y = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_QUATY, time);
	quaternion->z = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_QUATZ, time);
	quaternion->w = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_QUATW, time);
	*quaternion = limat_quaternion_normalize (*quaternion);
}

/** @} */
/** @} */

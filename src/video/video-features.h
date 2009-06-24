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
 * \addtogroup livid Video
 * @{
 * \addtogroup lividFeatures Features
 * @{
 */

#ifndef __VIDEO_FEATURES_H__
#define __VIDEO_FEATURES_H__

typedef struct _lividFeatures lividFeatures;
struct _lividFeatures
{
	int anisotropic_level;
	int shader_model;
	int max_texture_units;
};

extern lividFeatures livid_features;

void
livid_features_init ();

int
livid_features_get_max_samples ();

#endif

/** @} */
/** @} */

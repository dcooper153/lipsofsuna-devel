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
 * \addtogroup linet Network
 * @{
 * \addtogroup linetProtocol Protocol
 * @{
 */

#ifndef __NETWORK_PROTOCOL_H__
#define __NETWORK_PROTOCOL_H__

#define LINET_PROTOCOL_VERSION "E"
#define LINET_INVALID_ANIMATION 65535
#define LINET_INVALID_MODEL 65535

/****************************************************************************/
/* Object number ranges. */

#define LINET_RANGE_SERVER_START 0x00000000
#define LINET_RANGE_SERVER_END   0x0FFFFFFF
#define LINET_RANGE_CLIENT_START 0x10000000
#define LINET_RANGE_CLIENT_END   0x1FFFFFFF
#define LINET_RANGE_RENDER_START 0x20000000
#define LINET_RANGE_RENDER_END   0x2FFFFFFF

/****************************************************************************/
/* Network commands. */

enum
{
	/* Core. */
	LINET_SERVER_PACKET_RESOURCES,

	/* Extensions. */
	LIEXT_SKILLS_PACKET_RESET,
	LIEXT_SKILLS_PACKET_DIFF,
	LIEXT_SLOTS_PACKET_RESET,
	LIEXT_SLOTS_PACKET_DIFF,

	/* Custom. */
	LINET_SERVER_PACKET_CUSTOM = 128,

	LINET_SERVER_PACKET_MAX
};

enum
{
	LINET_CLIENT_PACKET_MOVE,

	LINET_EXT_CLIENT_PACKET_GENERATOR = 64,
	LINET_EXT_CLIENT_PACKET_EDITOR,

	LINET_CLIENT_PACKET_MAX
};

/****************************************************************************/
/* Object flags. */
/* FIXME */

enum
{
	LINET_OBJECT_FLAG_DYNAMIC = 0x01
};

/****************************************************************************/
/* Control flags. */
/* FIXME */

enum
{
	LINET_CONTROL_MOVE_FRONT = 0x01,
	LINET_CONTROL_MOVE_BACK  = 0x02,
	LINET_CONTROL_RUN        = 0x40,
	LINET_CONTROL_MASK       = 0xFF,
};

/****************************************************************************/
/* Effect flags. */
/* FIXME */

enum
{
	LINET_EFFECT_REPEAT = 0x01,

	LINET_EFFECT_DEFAULT = 0x00,
};

#endif

/** @} */
/** @} */

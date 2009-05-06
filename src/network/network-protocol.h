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
 * \addtogroup linet Network
 * @{
 * \addtogroup linetProtocol Protocol
 * @{
 */

#ifndef __NETWORK_PROTOCOL_H__
#define __NETWORK_PROTOCOL_H__

#define LINET_PROTOCOL_VERSION "D"
#define LINET_INVALID_ANIMATION 65535
#define LINET_INVALID_MODEL 65535

/****************************************************************************/
/* Object number ranges. */

#define LINET_RANGE_SERVER_START 0x00000000
#define LINET_RANGE_SERVER_END   0x0000EFFF
#define LINET_RANGE_CLIENT_START 0x0000FFFF
#define LINET_RANGE_CLIENT_END   0xFFFFFFFF

/****************************************************************************/
/* Network commands. */

enum
{
	/* Core. */
	LINET_SERVER_PACKET_ASSIGN,
	LINET_SERVER_PACKET_CHAT,
	LINET_SERVER_PACKET_OBJECT_ANIMATION,
	LINET_SERVER_PACKET_OBJECT_CREATE,
	LINET_SERVER_PACKET_OBJECT_DESTROY,
	LINET_SERVER_PACKET_OBJECT_EFFECT,
	LINET_SERVER_PACKET_OBJECT_GRAPHIC,
	LINET_SERVER_PACKET_OBJECT_SIMULATE,
	LINET_SERVER_PACKET_RESOURCES,

	/* Extensions. */
	LIEXT_INVENTORY_PACKET_RESET,
	LIEXT_INVENTORY_PACKET_DIFF,
	LIEXT_SKILLS_PACKET_RESET,
	LIEXT_SKILLS_PACKET_DIFF,
	LIEXT_SLOTS_PACKET_RESET,
	LIEXT_SLOTS_PACKET_DIFF,

	/* Custom. */
	LINET_SERVER_PACKET_CUSTOM = 128,

	LI_SERVER_COMMAND_LAST
};

enum
{
	LI_CLIENT_COMMAND_ACTION,
	LI_CLIENT_COMMAND_CHAT,
	LI_CLIENT_COMMAND_MOVE,

	LINET_EXT_CLIENT_PACKET_GENERATOR = 64,
	LINET_EXT_CLIENT_PACKET_EDITOR,

	LI_CLIENT_COMMAND_LAST
};

/****************************************************************************/
/* Assign packet flags. */
/* FIXME */

enum
{
	LI_FEATURE_ADMIN = 0x01,
	LI_FEATURE_MASK  = 0xFF,
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
	LI_CONTROL_MOVE_FRONT = 0x01,
	LI_CONTROL_MOVE_BACK  = 0x02,
	LI_CONTROL_RUN        = 0x40,
	LI_CONTROL_MASK       = 0xFF,
};

/****************************************************************************/
/* Effect flags. */
/* FIXME */

enum
{
	LI_EFFECT_REPEAT = 0x01,

	LI_EFFECT_DEFAULT = 0x00,
};

#endif

/** @} */
/** @} */

/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenAttachment Attachment
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-attachment.hpp"

LIRenAttachment::LIRenAttachment (LIRenObject* object)
{
	this->object = object;
	this->render = object->render;
	this->replacer = NULL;
}

LIRenAttachment::~LIRenAttachment ()
{
}

LIMdlNode* LIRenAttachment::find_node (const char* name)
{
	return NULL;
}

LIMdlModel* LIRenAttachment::get_model () const
{
	return NULL;
}

bool LIRenAttachment::has_model (LIRenModel* model)
{
	return false;
}

bool LIRenAttachment::is_loaded () const
{
	return true;
}

void LIRenAttachment::remove_model (LIRenModel* model)
{
}

void LIRenAttachment::update (float secs)
{
}

void LIRenAttachment::update_pose (LIMdlPoseSkeleton* skeleton)
{
}

void LIRenAttachment::update_settings ()
{
}

LIRenAttachment* LIRenAttachment::get_replacer ()
{
	return replacer;
}

void LIRenAttachment::set_replacer (LIRenAttachment* replacer)
{
	this->replacer = replacer;
}

/** @} */
/** @} */
/** @} */

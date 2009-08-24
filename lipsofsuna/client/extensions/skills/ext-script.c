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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSkills Skills
 * @{
 */

#include <client/lips-client.h>
#include <script/lips-script.h>
#include "ext-module.h"
#include "ext-widget.h"

/* @luadoc
 * module "Extension.Client.Skills"
 * ---
 * -- Display skills.
 * -- @name SkillWidget
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new skill widget.
 * --
 * -- @param self Skill widget class.
 * -- @param table Optional table of arguments.
 * -- @return New skill widget.
 * function SkillWidget.new(self, table)
 */
static int
SkillWidget_new (lua_State* lua)
{
	liextModule* module;
	liscrData* self;
	liscrScript* script;
	liwdgWidget* widget;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_SKILL_WIDGET);

	/* Allocate widget. */
	widget = liext_skill_widget_new (module);
	if (widget == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Allocate userdata. */
	self = liscr_data_new (script, widget, LIEXT_SCRIPT_SKILL_WIDGET);
	if (self == NULL)
	{
		liwdg_widget_free (widget);
		lua_pushnil (lua);
		return 1;
	}
	liwdg_widget_set_userdata (widget, self);

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);

	return 1;
}

/* @luadoc
 * ---
 * -- Monitored skill.
 * -- @name SkillWidget.skill
 * -- @class table
 */
static int
SkillWidget_setter_skill (lua_State* lua)
{
	const char* skill;
	liextSkillWidget* widget;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILL_WIDGET);
	skill = luaL_checkstring (lua, 3);
	widget = self->data;

	liext_skill_widget_set_skill (widget, 0, skill);
	return 0;
}

/*****************************************************************************/

void
liextSkillWidgetScript (liscrClass* self,
                        void*       data)
{
	liextModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_SKILL_WIDGET, data);
	liscr_class_inherit (self, licliWidgetScript, module->module);
	liscr_class_insert_func (self, "new", SkillWidget_new);
	liscr_class_insert_setter (self, "skill", SkillWidget_setter_skill);
}

/** @} */
/** @} */
/** @} */

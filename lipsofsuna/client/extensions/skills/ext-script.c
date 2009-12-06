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
#include "ext-skills.h"
#include "ext-widget.h"

/* @luadoc
 * module "Extension.Client.Skills"
 * ---
 * -- Query skills.
 * -- @name Skills
 * -- @class table
 */

/* @luadoc
 * ---
 * -- @brief Finds the skills for an object.
 * -- @param self Skills class.
 * -- @param object Object whose skills to find.
 * -- @return Skills or nil.
 * function Skills.find(self, object)
 */
static int
Skills_find (lua_State* lua)
{
	const char* name;
	liextModule* module;
	liextSkills* skills;
	liscrData* object;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_SKILLS);
	object = liscr_checkdata (lua, 2, LICOM_SCRIPT_OBJECT);
	if (!lua_isnoneornil (lua, 3))
		name = luaL_checkstring (lua, 3);
	else
		name = NULL;

	/* Find skills. */
	skills = liext_module_find_skills (module, LIENG_OBJECT (object->data)->id);
	if (skills == NULL)
		return 0;
	liscr_pushdata (lua, skills->script);

	return 1;
}

/* @luadoc
 * ---
 * -- @brief Gets the maximum value of a skill.
 * -- @param self Skills.
 * -- @param skill Skill name.
 * -- @return Number or nil.
 * function Skills.get_maximum(self, skill)
 */
static int
Skills_get_maximum (lua_State* lua)
{
	const char* name;
	liextSkill* skill;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILLS);
	name = luaL_checkstring (lua, 2);

	skill = liext_skills_find_skill (self->data, name);
	if (skill != NULL)
	{
		lua_pushnumber (lua, skill->maximum);
		return 1;
	}

	return 0;
}

/* @luadoc
 * ---
 * -- @brief Gets the value of a skill.
 * -- @param self Skills.
 * -- @param skill Skill name.
 * -- @return Number or nil.
 * function Skills.get_value(self, skill)
 */
static int
Skills_get_value (lua_State* lua)
{
	const char* name;
	liextSkill* skill;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILLS);
	name = luaL_checkstring (lua, 2);

	skill = liext_skills_find_skill (self->data, name);
	if (skill != NULL)
	{
		lua_pushnumber (lua, skill->value);
		return 1;
	}

	return 0;
}

/* @luadoc
 * ---
 * -- @brief Checks if a skill is present.
 * -- @param self Skills.
 * -- @param skill Skill name.
 * -- @return Boolean.
 * function Skills.has_skill(self, skill)
 */
static int
Skills_has_skill (lua_State* lua)
{
	const char* name;
	liscrData* self;
	liextSkill* skill;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILLS);
	name = luaL_checkstring (lua, 2);

	skill = liext_skills_find_skill (self->data, name);
	if (skill != NULL)
	{
		lua_pushboolean (lua, 1);
		return 1;
	}

	return 0;
}

/* @luadoc
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
	self = liscr_data_new (script, widget, LIEXT_SCRIPT_SKILL_WIDGET, licli_script_widget_free);
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
liextSkillsScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SKILLS, data);
	liscr_class_insert_func (self, "find", Skills_find);
	liscr_class_insert_func (self, "get_maximum", Skills_get_maximum);
	liscr_class_insert_func (self, "get_value", Skills_get_value);
	liscr_class_insert_func (self, "has_skill", Skills_has_skill);
}

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

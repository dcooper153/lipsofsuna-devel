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
 * --
 * -- Arguments:
 * -- object: Owner object.
 * --
 * -- @param self Skills class.
 * -- @param args Arguments.
 * -- @return Skills or nil.
 * function Skills.find(self, args)
 */
static void Skills_find (liscrArgs* args)
{
	liextModule* module;
	liextSkills* skills;
	liscrData* data;

	if (liscr_args_gets_data (args, "object", LICOM_SCRIPT_OBJECT, &data))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SKILLS);
		skills = liext_module_find_skills (module, LIENG_OBJECT (data->data)->id);
		if (skills != NULL)
			liscr_args_seti_data (args, skills->script);
	}
}

/* @luadoc
 * ---
 * -- @brief Gets the maximum value of a skill.
 * --
 * -- Arguments:
 * -- skill: Skill name.
 * --
 * -- @param self Skills.
 * -- @param args Arguments.
 * -- @return Number or nil.
 * function Skills.get_maximum(self, args)
 */
static void Skills_get_maximum (liscrArgs* args)
{
	const char* name;
	liextSkill* skill;

	if (liscr_args_gets_string (args, "skill", &name))
	{
		skill = liext_skills_find_skill (args->self, name);
		if (skill != NULL)
			liscr_args_seti_float (args, skill->maximum);
	}
}

/* @luadoc
 * ---
 * -- @brief Gets a list of skill names.
 * -- @param self Skills.
 * -- @return List of skill names.
 * function Skills.get_names(self)
 */
static void Skills_get_names (liscrArgs* args)
{
	lialgStrdicIter iter;
	liextSkills* skills;

	skills = args->self;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	LI_FOREACH_STRDIC (iter, skills->skills)
		liscr_args_seti_string (args, iter.key);
}

/* @luadoc
 * ---
 * -- @brief Gets the value of a skill.
 * --
 * -- Arguments:
 * -- skill: Skill name.
 * --
 * -- @param self Skills.
 * -- @param args Arguments.
 * -- @return Number or nil.
 * function Skills.get_value(self, argsl)
 */
static void Skills_get_value (liscrArgs* args)
{
	const char* name;
	liextSkill* skill;

	if (liscr_args_gets_string (args, "skill", &name))
	{
		skill = liext_skills_find_skill (args->self, name);
		if (skill != NULL)
			liscr_args_seti_float (args, skill->value);
	}
}

/* @luadoc
 * ---
 * -- @brief Checks if a skill is present.
 * --
 * -- Arguments:
 * -- skill: Skill name.
 * --
 * -- @param self Skills.
 * -- @param args Arguments.
 * -- @return Boolean.
 * function Skills.has_skill(self, args)
 */
static void Skills_has_skill (liscrArgs* args)
{
	const char* name;
	liextSkill* skill;

	if (liscr_args_gets_string (args, "skill", &name))
	{
		skill = liext_skills_find_skill (args->self, name);
		if (skill != NULL)
			liscr_args_seti_bool (args, 1);
	}
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
 * -- @param args Arguments.
 * -- @return New skill widget.
 * function SkillWidget.new(self, args)
 */
static void SkillWidget_new (liscrArgs* args)
{
	liextModule* module;
	liscrData* data;
	liwdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SKILL_WIDGET);
	self = liext_skill_widget_new (module);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_SKILL_WIDGET, licli_script_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * ---
 * -- Monitored skill.
 * -- @name SkillWidget.skill
 * -- @class table
 */
static void SkillWidget_setter_skill (liscrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		liext_skill_widget_set_skill (args->self, 0, value);
}

/*****************************************************************************/

void
liextSkillsScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SKILLS, data);
	liscr_class_insert_cfunc (self, "find", Skills_find);
	liscr_class_insert_mfunc (self, "get_maximum", Skills_get_maximum);
	liscr_class_insert_mfunc (self, "get_names", Skills_get_names);
	liscr_class_insert_mfunc (self, "get_value", Skills_get_value);
	liscr_class_insert_mfunc (self, "has_skill", Skills_has_skill);
}

void
liextSkillWidgetScript (liscrClass* self,
                        void*       data)
{
	liextModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_SKILL_WIDGET, data);
	liscr_class_inherit (self, licliWidgetScript, module->module);
	liscr_class_insert_cfunc (self, "new", SkillWidget_new);
	liscr_class_insert_mvar (self, "skill", NULL, SkillWidget_setter_skill);
}

/** @} */
/** @} */
/** @} */

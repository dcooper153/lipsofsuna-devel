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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtSkills Skills
 * @{
 */

#include <lipsofsuna/main.h>
#include <lipsofsuna/server.h>
#include "ext-module.h"
#include "ext-skills.h"

/* @luadoc
 * module "Extension.Server.Skills"
 * ---
 * -- Manage skills.
 * -- @name Skills
 * -- @class table
 */

/* @luadoc
 * ---
 * -- @brief Check if the object has enough skill.
 * --
 * -- Arguments:
 * -- owner: Object. (required)
 * -- skill: Skill name.
 * -- min: Minimum required value.
 * --
 * -- @param self Skills class.
 * -- @param args Arguments.
 * -- @return True if the user had enough skill.
 * function Skills.check(self, args)
 */
static void Skills_check (LIScrArgs* args)
{
	float value;
	const char* name;
	LIExtModule* module;
	LIExtSkill* skill;
	LIExtSkills* skills;
	LIScrData* data;

	if (!liscr_args_gets_string (args, "skill", &name) ||
	    !liscr_args_gets_float (args, "min", &value))
	{
		liscr_args_seti_bool (args, 1);
		return;
	}
	if (liscr_args_gets_data (args, "owner", LISCR_SCRIPT_OBJECT, &data))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SKILLS);
		skills = liext_module_find_skills (module, data->data);
		if (skills == NULL)
			return;
		skill = liext_skills_find_skill (skills, name);
		if (skill == NULL)
			return;
		if (skill->value >= value)
			liscr_args_seti_bool (args, 1);
	}
}

/* @luadoc
 * ---
 * -- @brief Finds the skills for an object.
 * --
 * -- Arguments:
 * -- owner: Object. (required)
 * --
 * -- @param self Skills class.
 * -- @param args Arguments.
 * -- @return Skills or nil.
 * function Skills.find(self, args)
 */
static void Skills_find (LIScrArgs* args)
{
	LIExtModule* module;
	LIExtSkills* skills;
	LIScrData* data;

	if (liscr_args_gets_data (args, "owner", LISCR_SCRIPT_OBJECT, &data))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SKILLS);
		skills = liext_module_find_skills (module, data->data);
		if (skills != NULL)
			liscr_args_seti_data (args, skills->script);
	}
}

/* @luadoc
 * ---
 * -- Creates a new skills list.
 * --
 * -- @param self Skills class.
 * -- @param args Arguments.
 * -- @return New skills.
 * function Skills.new(self, args)
 */
static void Skills_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIExtSkills* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SKILLS);
	self = liext_skills_new (module);
	if (self == NULL)
		return;

	/* Configure userdata. */
	liscr_args_call_setters (args, self->script);
	liscr_args_seti_data (args, self->script);
	liscr_data_unref (self->script, NULL);
}

/* @luadoc
 * ---
 * -- @brief Tries to subtract a value from the specified skill.
 * --
 * -- Arguments:
 * -- owner: Object. (required)
 * -- skill: Skill name. (required)
 * -- value: Value to subtract. (required)
 * --
 * -- @param self Skills class.
 * -- @param args Arguments.
 * -- @return True if the user had enough skill.
 * function Skills.subtract(self, args)
 */
static void Skills_subtract (LIScrArgs* args)
{
	float value;
	const char* name;
	LIExtModule* module;
	LIExtSkill* skill;
	LIExtSkills* skills;
	LIScrData* data;

	if (liscr_args_gets_string (args, "skill", &name) &&
	    liscr_args_gets_float (args, "value", &value) &&
	    liscr_args_gets_data (args, "owner", LISCR_SCRIPT_OBJECT, &data))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SKILLS);
		skills = liext_module_find_skills (module, data->data);
		if (skills == NULL)
			return;
		skill = liext_skills_find_skill (skills, name);
		if (skill == NULL || skill->value < value)
			return;
		liext_skill_set_value (skill, skill->value - value);
		liscr_args_seti_bool (args, 1);
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
 * -- @param skill Skill name.
 * -- @return Number or nil.
 * function Skills.get_maximum(self, skill)
 */
static void Skills_get_maximum (LIScrArgs* args)
{
	const char* name;
	LIExtSkill* skill;

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
static void Skills_get_names (LIScrArgs* args)
{
	LIAlgStrdicIter iter;
	LIExtSkills* skills;

	skills = args->self;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	LIALG_STRDIC_FOREACH (iter, skills->skills)
		liscr_args_seti_string (args, iter.key);
}

/* @luadoc
 * ---
 * -- @brief Gets the regenreation speed of a skill.
 * --
 * -- Arguments:
 * -- skill: Skill name.
 * --
 * -- @param self Skills.
 * -- @param args Arguments.
 * -- @return Number or nil.
 * function Skills.get_regen(self, args)
 */
static void Skills_get_regen (LIScrArgs* args)
{
	const char* name;
	LIExtSkill* skill;

	if (liscr_args_gets_string (args, "skill", &name))
	{
		skill = liext_skills_find_skill (args->self, name);
		if (skill != NULL)
			liscr_args_seti_float (args, skill->regen);
	}
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
 * function Skills.get_value(self, args)
 */
static void Skills_get_value (LIScrArgs* args)
{
	const char* name;
	LIExtSkill* skill;

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
static void Skills_has_skill (LIScrArgs* args)
{
	const char* name;
	LIExtSkill* skill;

	if (liscr_args_gets_string (args, "skill", &name))
	{
		skill = liext_skills_find_skill (args->self, name);
		if (skill != NULL)
			liscr_args_seti_bool (args, 1);
	}
}

/* @luadoc
 * ---
 * -- Registers a skill.
 * --
 * -- Arguments:
 * -- skill: Skill name. (required)
 * -- prot: Protection type. ("public"/"private"/"internal")
 * -- maximum: Maximum value.
 * -- value: Initial value.
 * -- regen: Regeneration rate.
 * --
 * -- @param self Skills.
 * -- @param type Skill protection type.
 * -- @param name Skill name.
 * function Skills.register(self, type, name)
 */
static void Skills_register (LIScrArgs* args)
{
	int type;
	float value;
	const char* name;
	const char* prot;
	LIExtSkill* skill;

	if (!liscr_args_gets_string (args, "name", &name))
		return;

	/* Create skill. */
	/* TODO: Override type if different and inform clients? */
	skill = liext_skills_find_skill (args->self, name);
	if (skill == NULL)
	{
		type = LIEXT_SKILL_TYPE_PRIVATE;
		if (liscr_args_gets_string (args, "prot", &prot))
		{
			if (!strcmp (prot, "public"))
				type = LIEXT_SKILL_TYPE_PUBLIC;
			else if (!strcmp (prot, "internal"))
				type = LIEXT_SKILL_TYPE_INTERNAL;
		}
		liext_skills_insert_skill (args->self, type, name);
		skill = liext_skills_find_skill (args->self, name);
		if (skill == NULL)
			return;
	}

	/* Set optional values. */
	if (liscr_args_gets_float (args, "maximum", &value))
		liext_skill_set_maximum (skill, value);
	if (liscr_args_gets_float (args, "regen", &value))
		liext_skill_set_regen (skill, value);
	if (liscr_args_gets_float (args, "value", &value))
		liext_skill_set_value (skill, value);
}

/* @luadoc
 * ---
 * -- @brief Sets the maximum value of a skill.
 * --
 * -- Arguments:
 * -- skill: Skill name.
 * -- value: Number.
 * --
 * -- @param self Skills.
 * -- @param args Arguments.
 * function Skills.set_maximum(self, args)
 */
static void Skills_set_maximum (LIScrArgs* args)
{
	float value;
	const char* name;
	LIExtSkill* skill;

	if (liscr_args_gets_string (args, "skill", &name) &&
	    liscr_args_gets_float (args, "value", &value))
	{
		skill = liext_skills_find_skill (args->self, name);
		if (skill != NULL)
			liext_skill_set_maximum (skill, value);
	}
}

/* @luadoc
 * ---
 * -- @brief Sets the regeneration speed of a skill.
 * --
 * -- Arguments:
 * -- skill: Skill name.
 * -- value: Number.
 * --
 * -- @param self Skills.
 * -- @param args Arguments.
 * function Skills.set_regen(self, args)
 */
static void Skills_set_regen (LIScrArgs* args)
{
	float value;
	const char* name;
	LIExtSkill* skill;

	if (liscr_args_gets_string (args, "skill", &name) &&
	    liscr_args_gets_float (args, "value", &value))
	{
		skill = liext_skills_find_skill (args->self, name);
		if (skill != NULL)
			liext_skill_set_regen (skill, value);
	}
}

/* @luadoc
 * ---
 * -- @brief Sets the value of a skill.
 * --
 * -- Arguments:
 * -- skill: Skill name.
 * -- value: Number.
 * --
 * -- @param self Skills.
 * -- @param args Arguments.
 * function Skills.set_value(self, args)
 */
static void Skills_set_value (LIScrArgs* args)
{
	float value;
	const char* name;
	LIExtSkill* skill;

	if (liscr_args_gets_string (args, "skill", &name) &&
	    liscr_args_gets_float (args, "value", &value))
	{
		skill = liext_skills_find_skill (args->self, name);
		if (skill != NULL)
			liext_skill_set_value (skill, value);
	}
}

/* @luadoc
 * ---
 * -- Owner object.
 * -- @name Skills.owner
 * -- @class table
 */
static void Skills_getter_owner (LIScrArgs* args)
{
	LIEngObject* object;

	object = liext_skills_get_owner (args->self);
	if (object != NULL)
		liscr_args_seti_data (args, object->script);
}
static void Skills_setter_owner (LIScrArgs* args)
{
	LIScrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_OBJECT, &data))
		liext_skills_set_owner (args->self, data->data);
	else
		liext_skills_set_owner (args->self, NULL);
}

/*****************************************************************************/

void
liext_script_skills (LIScrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SKILLS, data);
	liscr_class_insert_cfunc (self, "check", Skills_check);
	liscr_class_insert_cfunc (self, "find", Skills_find);
	liscr_class_insert_cfunc (self, "new", Skills_new);
	liscr_class_insert_cfunc (self, "subtract", Skills_subtract);
	liscr_class_insert_mfunc (self, "get_maximum", Skills_get_maximum);
	liscr_class_insert_mfunc (self, "get_names", Skills_get_names);
	liscr_class_insert_mfunc (self, "get_regen", Skills_get_regen);
	liscr_class_insert_mfunc (self, "get_value", Skills_get_value);
	liscr_class_insert_mfunc (self, "has_skill", Skills_has_skill);
	liscr_class_insert_mfunc (self, "register", Skills_register);
	liscr_class_insert_mfunc (self, "set_maximum", Skills_set_maximum);
	liscr_class_insert_mfunc (self, "set_regen", Skills_set_regen);
	liscr_class_insert_mfunc (self, "set_value", Skills_set_value);
	liscr_class_insert_mvar (self, "owner", Skills_getter_owner, Skills_setter_owner);
}

/** @} */
/** @} */

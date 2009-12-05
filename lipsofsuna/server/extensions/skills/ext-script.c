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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvSkills Skills
 * @{
 */

#include <script/lips-script.h>
#include <server/lips-server.h>
#include "ext-module.h"
#include "ext-skills.h"

/* @luadoc
 * module "Extension.Server.Skills"
 * ---
 * -- Create numeric skills and stats.
 * -- @name Skill
 * -- @class table
 */

static int
Skill___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_isdata (lua, 1, LIEXT_SCRIPT_SKILL);

	liscr_data_free (self);
	return 0;
}

/* @luadoc
 * ---
 * -- Target value.
 * -- @name Skill.maximum
 * -- @class table
 */
static int
Skill_getter_maximum (lua_State* lua)
{
	liextSkill* skill;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILL);
	skill = self->data;

	lua_pushnumber (lua, skill->maximum);
	return 1;
}
static int
Skill_setter_maximum (lua_State* lua)
{
	float value;
	liextSkill* skill;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILL);
	value = luaL_checknumber (lua, 3);
	skill = self->data;

	liext_skill_set_maximum (skill, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Regeneration rate.
 * -- @name Skill.regen
 * -- @class table
 */
static int
Skill_getter_regen (lua_State* lua)
{
	liextSkill* skill;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILL);
	skill = self->data;

	lua_pushnumber (lua, skill->regen);
	return 1;
}
static int
Skill_setter_regen (lua_State* lua)
{
	float value;
	liextSkill* skill;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILL);
	value = luaL_checknumber (lua, 3);
	skill = self->data;

	liext_skill_set_regen (skill, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Current value.
 * -- @name Skill.value
 * -- @class table
 */
static int
Skill_getter_value (lua_State* lua)
{
	liextSkill* skill;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILL);
	skill = self->data;

	lua_pushnumber (lua, skill->value);
	return 1;
}
static int
Skill_setter_value (lua_State* lua)
{
	float value;
	liextSkill* skill;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILL);
	value = luaL_checknumber (lua, 3);
	skill = self->data;

	liext_skill_set_value (skill, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Manage skills.
 * -- @name Skills
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Skill type for internal skills.
 * --
 * -- Internal skills are never sent to clients.
 * --
 * -- @name Skills.INTERNAL
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Skill type for private skills.
 * --
 * -- Private skills only visible to the owner of the skills.
 * --
 * -- @name Skills.PRIVATE
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Skill type for public skills.
 * --
 * -- Public skills only visible to all clients.
 * --
 * -- @name Skills.PUBLIC
 * -- @class table
 */

static int
Skills___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_isdata (lua, 1, LIEXT_SCRIPT_SKILLS);

	liext_skills_free (self->data);
	liscr_data_free (self);
	return 0;
}

static int
Skills___index (lua_State* lua)
{
	const char* name;
	liscrData* self;
	liextSkill* skill;

	/* Try skills first. */
	if (lua_isstring (lua, 2))
	{
		self = liscr_isdata (lua, 1, LIEXT_SCRIPT_SKILLS);
		if (self != NULL)
		{
			name = lua_tostring (lua, 2);
			skill = liext_skills_find_skill (self->data, name);
			if (skill != NULL)
			{
				liscr_pushdata (lua, skill->script);
				return 1;
			}
		}
	}

	return liscr_class_default___index (lua);
}

static int
Skills___newindex (lua_State* lua)
{
	const char* name;
	liscrData* self;
	liextSkill* skill;

	/* Protect skills. */
	if (lua_isstring (lua, 2))
	{
		self = liscr_isdata (lua, 1, LIEXT_SCRIPT_SKILLS);
		if (self != NULL)
		{
			name = lua_tostring (lua, 2);
			skill = liext_skills_find_skill (self->data, name);
			luaL_argcheck (lua, skill == NULL, 2, "attempt to overwrite skill field");
		}
	}

	return liscr_class_default___newindex (lua);
}

/*
 * ---
 * -- @brief Check if the object has enough skill.
 * -- @param self Skills class.
 * -- @param object Object whose skills to check.
 * -- @param skill Skill name.
 * -- @param value Required value.
 * -- @return True if the user had enough skill.
 * function Skills.check(self, object, skill, value)
 */
static int
Skills_check (lua_State* lua)
{
	float value;
	const char* name;
	liextModule* module;
	liextSkill* skill;
	liextSkills* skills;
	liscrData* object;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_SKILLS);
	object = liscr_checkdata (lua, 2, LICOM_SCRIPT_OBJECT);
	name = luaL_checkstring (lua, 3);
	value = luaL_checknumber (lua, 4);

	skills = liext_module_find_skills (module, object->data);
	if (skills == NULL)
		return 0;
	skill = liext_skills_find_skill (skills, name);
	if (skill == NULL)
		return 0;
	if (skill->value < value)
		return 0;
	lua_pushboolean (lua, 1);

	return 1;
}

/*
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
	liextModule* module;
	liextSkills* skills;
	liscrData* object;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_SKILLS);
	object = liscr_checkdata (lua, 2, LICOM_SCRIPT_OBJECT);

	skills = liext_module_find_skills (module, object->data);
	if (skills == NULL)
		return 0;
	liscr_pushdata (lua, skills->data);

	return 1;
}

/* @luadoc
 * ---
 * -- Creates a new skills list.
 * --
 * -- @param self Skills class.
 * -- @param table Optional table of arguments.
 * -- @return New skills.
 * function Skills.new(self, table)
 */
static int
Skills_new (lua_State* lua)
{
	liextModule* module;
	liextSkills* skills;
	liscrData* self;
	liscrScript* script = liscr_script (lua);

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_SKILLS);

	/* Allocate self. */
	skills = liext_skills_new (module);
	if (skills == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	self = liscr_data_new (script, skills, LIEXT_SCRIPT_SKILLS);
	if (self == NULL)
	{
		liext_skills_free (skills);
		lua_pushnil (lua);
		return 1;
	}
	skills->data = self;

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);
	return 1;
}

/* @luadoc
 * ---
 * -- Registers a skill.
 * --
 * -- @param self Skills.
 * -- @param type Skill protection type.
 * -- @param name Skill name.
 * function Skills.register(self, type, name)
 */
static int
Skills_register (lua_State* lua)
{
	int type;
	const char* name;
	liscrData* self;
	liextSkill* skill;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILLS);
	type = (int) luaL_checknumber (lua, 2);
	name = luaL_checkstring (lua, 3);
	luaL_argcheck (lua, type >= 0 && type <= LIEXT_SKILL_TYPE_MAX, 2, "invalid skill type");

	/* Check for existing skill. */
	/* TODO: Override type if different and inform clients? */
	skill = liext_skills_find_skill (self->data, name);
	if (skill != NULL)
		return 0;

	/* Create the slot. */
	liext_skills_insert_skill (self->data, type, name);

	return 0;
}

/*
 * ---
 * -- @brief Tries to subtract a value from the specified skill.
 * -- @param self Skills class.
 * -- @param object Object whose skills to modify.
 * -- @param skill Skill name.
 * -- @param value Value to subtract.
 * -- @return True if the user had enough skill.
 * function Skills.subtract(self, object, skill, value)
 */
static int
Skills_subtract (lua_State* lua)
{
	float value;
	const char* name;
	liextModule* module;
	liextSkill* skill;
	liextSkills* skills;
	liscrData* object;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_SKILLS);
	object = liscr_checkdata (lua, 2, LICOM_SCRIPT_OBJECT);
	name = luaL_checkstring (lua, 3);
	value = luaL_checknumber (lua, 4);

	skills = liext_module_find_skills (module, object->data);
	if (skills == NULL)
		return 0;
	skill = liext_skills_find_skill (skills, name);
	if (skill == NULL)
		return 0;
	if (skill->value < value)
		return 0;
	liext_skill_set_value (skill, skill->value - value);
	lua_pushboolean (lua, 1);

	return 1;
}

/* @luadoc
 * ---
 * -- Owner object.
 * -- @name Skills.owner
 * -- @class table
 */
static int
Skills_getter_owner (lua_State* lua)
{
	liengObject* object;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILLS);

	object = liext_skills_get_owner (self->data);
	if (object != NULL && object->script != NULL)
		liscr_pushdata (lua, object->script);
	else
		lua_pushnil (lua);
	return 1;
}
static int
Skills_setter_owner (lua_State* lua)
{
	liscrData* object;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SKILLS);
	object = liscr_checkdata (lua, 3, LICOM_SCRIPT_OBJECT);

	liext_skills_set_owner (self->data, object->data);
	return 0;
}

/*****************************************************************************/

void
liextSkillScript (liscrClass* self,
                  void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SKILL, data);
	liscr_class_insert_func (self, "__gc", Skill___gc);
	liscr_class_insert_getter (self, "maximum", Skill_getter_maximum);
	liscr_class_insert_getter (self, "regen", Skill_getter_regen);
	liscr_class_insert_getter (self, "value", Skill_getter_value);
	liscr_class_insert_setter (self, "maximum", Skill_setter_maximum);
	liscr_class_insert_setter (self, "regen", Skill_setter_regen);
	liscr_class_insert_setter (self, "value", Skill_setter_value);
}

void
liextSkillsScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SKILLS, data);
	liscr_class_insert_enum (self, "INTERNAL", LIEXT_SKILL_TYPE_INTERNAL);
	liscr_class_insert_enum (self, "PRIVATE", LIEXT_SKILL_TYPE_PRIVATE);
	liscr_class_insert_enum (self, "PUBLIC", LIEXT_SKILL_TYPE_PUBLIC);
	liscr_class_insert_func (self, "__gc", Skills___gc);
	liscr_class_insert_func (self, "__index", Skills___index);
	liscr_class_insert_func (self, "__newindex", Skills___newindex);
	liscr_class_insert_func (self, "check", Skills_check);
	liscr_class_insert_func (self, "find", Skills_find);
	liscr_class_insert_func (self, "new", Skills_new);
	liscr_class_insert_func (self, "register", Skills_register);
	liscr_class_insert_func (self, "subtract", Skills_subtract);
	liscr_class_insert_getter (self, "owner", Skills_getter_owner);
	liscr_class_insert_setter (self, "owner", Skills_setter_owner);
}

/** @} */
/** @} */
/** @} */

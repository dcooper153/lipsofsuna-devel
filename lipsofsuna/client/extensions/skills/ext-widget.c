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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSkills Skills
 * @{
 */

#include <client/lips-client.h>
#include "ext-skills.h"
#include "ext-widget.h"

static const void*
private_base ();

static int
private_init (liextSkillWidget* self,
              liwdgManager*     manager);

static void
private_free (liextSkillWidget* self);

static int
private_event (liextSkillWidget* self,
               liwdgEvent*       event);

/****************************************************************************/

const liwdgClass liextSkillWidgetType =
{
	LIWDG_BASE_DYNAMIC, private_base, "SkillWidget", sizeof (liextSkillWidget),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

liwdgWidget*
liext_skill_widget_new (liextModule* module)
{
	liwdgWidget* self;

	self = liwdg_widget_new (module->client->widgets, &liextSkillWidgetType);
	if (self == NULL)
		return NULL;
	LIEXT_SKILL_WIDGET (self)->module = module;

	return self;
}

int
liext_skill_widget_set_skill (liextSkillWidget* self,
                              uint32_t          object,
                              const char*       name)
{
	char* tmp;

	tmp = listr_dup (name);
	if (tmp == NULL)
		return 0;
	lisys_free (self->skill);
	self->skill = tmp;
	self->object = object;

	return 1;
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgGroupType;
}

static int
private_init (liextSkillWidget* self,
              liwdgManager*     manager)
{
	int i;
	liwdgWidget* widgets[] =
	{
		liwdg_progress_new (manager)
	};

	/* Check memory. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 1, 1))
		goto error;
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (liwdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			goto error;
	}

	/* Assign widgets. */
	self->value = widgets[(i = 0)];

	/* Pack widgets. */
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, self->value);
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 0, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self), 0, 1);

	return 1;

error:
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (liwdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			liwdg_widget_free (widgets[i]);
	}
	return 0;
}

static void
private_free (liextSkillWidget* self)
{
	lisys_free (self->skill);
}

static int
private_event (liextSkillWidget* self,
               liwdgEvent*       event)
{
	uint32_t id;
	char buffer[256];
	liengObject* object;
	liextSkill* skill;
	liextSkills* skills;

	/* Update value. */
	if (event->type == LIWDG_EVENT_TYPE_UPDATE)
	{
		liwdgGroupType.event (LIWDG_WIDGET (self), event);
		if (!self->object || self->skill == NULL)
		{
			if (self->module->client->network == NULL)
				goto disable;
			id = self->module->client->network->id;
		}
		else
			id = self->object;
		object = lieng_engine_find_object (self->module->client->engine, id);
		if (object == NULL)
			goto disable;
		skills = liext_module_find_skills (self->module, id);
		if (skills == NULL)
			goto disable;
		skill = liext_skills_find_skill (skills, self->skill);
		if (skill == NULL)
			goto disable;
		snprintf (buffer, 256, "%d/%d", (int) skill->value, (int) skill->maximum);
		liwdg_progress_set_value (LIWDG_PROGRESS (self->value), skill->value / skill->maximum);
		liwdg_progress_set_text (LIWDG_PROGRESS (self->value), buffer);
		return 1;
	}

	return liwdgGroupType.event (LIWDG_WIDGET (self), event);

disable:
	liwdg_progress_set_value (LIWDG_PROGRESS (self->value), 0.0f);
	return 1;
}

/** @} */
/** @} */
/** @} */

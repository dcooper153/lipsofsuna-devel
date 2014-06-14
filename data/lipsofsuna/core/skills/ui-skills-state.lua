-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local UiSkillWidget = require("core/skills/ui-skill-widget")

Ui:add_state{
	state = "skills",
	label = "Skills",
	init = function()
		-- Create a sorted list of skills and their toggle states.
		local status = Main.client_skills:get_statuses()
		local skills = {}
		for name,status in pairs(status) do
			local unlocked = Client.data.unlocks:get("skill", name)
			if unlocked and status ~= "incompatible" then
				local spec = Main.specs:find_by_name("SkillSpec", name)
				local active = (status == "active")
				local activable = (status == "activable")
				table.insert(skills, {name, spec, active or activable, active})
			end
		end
		table.sort(skills, function(a,b) return a[1] < b[1] end)
		-- Create the skill toggles.
		local widgets = {}
		for k,v in pairs(skills) do
			table.insert(widgets, UiSkillWidget(v[2], v[3], v[4]))
		end
		return widgets
	end}

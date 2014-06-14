-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local Client = require("core/client/client")
local Skills = require("core/skills/skills")

Client.reset_hooks:register(0, function()
	Main.client_skills = Skills()
	for k,v in pairs(Main.specs:get_spec_names("SkillSpec")) do
		local found = false
		for k1,v1 in pairs(v.requires) do found = true end
		Main.client_skills.skills[k] = {active = not found, value = false}
	end
end)

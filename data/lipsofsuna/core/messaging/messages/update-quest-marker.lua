-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local QuestSpec = require("core/specs/quest")

Main.messaging:register_message{
	name = "update quest marker",
	server_to_client_encode = function(self, name, marker)
		return {"string", name, "string", marker}
	end,
	server_to_client_decode = function(self, packet)
		local ok,name,marker = packet:read("string", "string")
		if not ok then return end
		return {name, marker}
	end,
	server_to_client_handle = function(self, name, marker)
		-- Find the quest spec.
		local spec = QuestSpec:find_by_name(name)
		if not spec then return end
		-- Find or create the quest.
		local quest = Operators.quests:get_quest_by_name(name)
		if not quest then
			quest = {name = name, spec = spec, status = "inactive", text = ""}
			Operators.quests:add_quest(quest)
		end
		-- Update quest data.
		if marker and marker ~= "" then
			quest.marker = marker
		else
			quest.marker = nil
		end
	end}

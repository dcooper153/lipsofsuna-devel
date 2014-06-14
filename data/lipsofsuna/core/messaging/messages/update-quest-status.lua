-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local status_name_to_number = {["inactive"] = 0, ["active"] = 1, ["completed"] = 2}
local status_number_to_name = {[0] = "inactive", [1] = "active", [2] = "completed"}

Main.messaging:register_message{
	name = "update quest status",
	server_to_client_encode = function(self, name, status, text)
		return {"string", name, "uint8", status_name_to_number[status] or 0, "string", text or ""}
	end,
	server_to_client_decode = function(self, packet)
		local ok,name,status,text = packet:read("string", "uint8", "string")
		if not ok then return end
		status = status_number_to_name[status]
		if not status then return end
		return {name, status, text}
	end,
	server_to_client_handle = function(self, name, status, text)
		-- Find the quest spec.
		local spec = Main.specs:find_by_name("QuestSpec", name)
		if not spec then return end
		-- Find or create the quest.
		local quest = Operators.quests:get_quest_by_name(name)
		if not quest then
			quest = {name = name, spec = spec, status = "inactive", text = ""}
			Operators.quests:add_quest(quest)
		end
		-- Update the quest.
		Operators.quests:set_quest_status(quest, status or "inactive", text or "")
		-- Update the user interface.
		if Ui:get_state() == "quests" then
			Ui:restart_state()
		end
	end}

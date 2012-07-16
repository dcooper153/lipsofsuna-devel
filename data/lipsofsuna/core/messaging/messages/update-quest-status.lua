local status_name_to_number = {["inactive"] = 0, ["active"] = 1, ["completed"] = 2}
local status_number_to_name = {[0] = "inactive", [1] = "active", [2] = "completed"}

Message{
	name = "update quest status",
	server_to_client_encode = function(self, id, status, text)
		return {"uint32", id, "uint8", status_name_to_number[status] or 0, "string", text or ""}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,status,text = packet:read("uint32", "uint8", "string")
		if not ok then return end
		status = status_number_to_name[status]
		if not status then return end
		return {id, status, text}
	end,
	server_to_client_handle = function(self, id, status, text)
		-- Find the quest spec.
		local spec = Quest:find{id = id}
		if not spec then return end
		-- Find or create the quest.
		local quest = Operators.quests:get_quest_by_id(id)
		if not quest then
			quest = {id = id, spec = spec, status = "inactive", text = ""}
			Operators.quests:add_quest(quest)
		end
		-- Update the quest.
		Operators.quests:set_quest_status(quest, status, text)
		-- Update the user interface.
		if Ui.state == "quests" then
			Ui:restart_state()
		end
	end}

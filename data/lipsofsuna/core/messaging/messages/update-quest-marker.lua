Message{
	name = "update quest marker",
	server_to_client_encode = function(self, id, name)
		return {"uint32", id, "string", name}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,name = packet:read("uint32", "string")
		if not ok then return end
		return {id, name}
	end,
	server_to_client_handle = function(self, id, name)
		-- Find the quest spec.
		local spec = Quest:find{id = id}
		if not spec then return end
		-- Find or create the quest.
		local quest = Operators.quests:get_quest_by_id(id)
		if not quest then
			quest = {id = id, spec = spec, status = "inactive", text = ""}
			Operators.quests:add_quest(quest)
		end
		-- Update quest data.
		if name ~= "" then
			quest.marker = name
		else
			quest.marker = nil
		end
	end}

Message{
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
		local spec = Questspec:find{name = name}
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

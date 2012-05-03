Protocol:add_handler{type = "QUEST_MARKER", func = function(event)
	-- Read the packet.
	local ok,id,n = event.packet:read("uint32", "string")
	if not ok then return end
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
	if n ~= "" then
		quest.marker = n
	else
		quest.marker = nil
	end
end}

Protocol:add_handler{type = "QUEST_STATUS", func = function(event)
	-- Read the packet.
	local ok,id,c,t = event.packet:read("uint32", "uint8", "string")
	if not ok then return end
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
	if c == 0 then
		Operators.quests:set_quest_status(quest, "active", t)
	elseif c == 1 then
		Operators.quests:set_quest_status(quest, "completed", t)
	end
	-- Update the user interface.
	if Ui.state == "quests" then
		Ui:restart_state()
	end
end}

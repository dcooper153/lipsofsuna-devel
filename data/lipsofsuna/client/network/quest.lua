Protocol:add_handler{type = "QUEST_MARKER", func = function(event)
	-- Read the packet.
	local ok,id,n = event.packet:read("uint32", "string")
	if not ok then return end
	-- Find the quest spec.
	local spec = Quest:find{id = id}
	if not spec then return end
	-- Find or create the quest.
	local quest = Client.data.quests.quests[id]
	if not quest then
		quest = {id = id, spec = spec, status = "inactive", text = ""}
		Client.data.quests.quests[id] = quest
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
	local quest = Client.data.quests.quests[id]
	if not quest then
		quest = {id = id, spec = spec, status = "inactive", text = ""}
		Client.data.quests.quests[id] = quest
	end
	-- Update quest data.
	if quest.status == "inactive" and c == 0 then
		Client:append_log("Started quest: " .. spec.name)
		quest.status = "active"
		quest.text = t
	elseif quest.status ~= "completed" and c == 1 then
		Client:append_log("Completed quest: " .. spec.name)
		quest.status = "completed"
		quest.text = t
	elseif quest.text ~= t then
		Client:append_log("Updated quest: " .. spec.name)
		quest.text = t
	else
		return
	end
	-- Play a sound effect unless not played too recently.
	if Program.time - Client.data.quests.sound_timer > 2 then
		Client.data.quests.sound_timer = Program.time
		Effect:play("quest1")
	end
	-- Update the user interface.
	if Ui.state == "quests" then
		Ui:restart_state()
	end
end}

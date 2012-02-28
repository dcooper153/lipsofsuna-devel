-- Sets to position of the quest marker.
Protocol:add_handler{type = "QUEST_MARKER", func = function(event)
	local ok,id,n = event.packet:read("uint32", "string")
	if not ok then return end
	local quest = Quest:find{id = id}
	if not quest then return end
	if n ~= "" then
		quest.marker = n
	else
		quest.marker = nil
	end
end}

-- Updates the quest status.
Protocol:add_handler{type = "QUEST_STATUS", func = function(event)
	local ok,id,c,t = event.packet:read("uint32", "uint8", "string")
	if not ok then return end
	-- Update the quest.
	local quest = Quest:find{id = id}
	if not quest then return end
	if quest.status == "inactive" and c == 0 then
		Client:append_log("Started quest: " .. quest.name)
		quest.status = "active"
		quest.text = t
		Client.views.quests:update(quest)
	elseif quest.status ~= "completed" and c == 1 then
		Client:append_log("Completed quest: " .. quest.name)
		quest.status = "completed"
		quest.text = t
		Client.views.quests:update(quest)
	elseif quest.text ~= t then
		Client:append_log("Updated quest: " .. quest.name)
		quest.text = t
		Client.views.quests:update(quest)
	end
	-- Play a sound effect unless not played too recently.
	if Program.time - Client.views.quests.sound_play_time > 2 then
		Client.views.quests.sound_play_time = Program.time
		Effect:play("quest1")
	end
end}

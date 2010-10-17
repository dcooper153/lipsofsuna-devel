-- Updates the quest status.
Protocol:add_handler{type = "QUEST_STATUS", func = function(event)
	local ok,id,c,t = event.packet:read("uint32", "uint8", "string")
	if ok then
		local quest = Quest:find{id = id}
		if not quest then return end
		if quest.status == "inactive" and c == 0 then
			Gui.chat_history:append{text = "Started quest: " .. quest.name}
			quest.status = "active"
			quest.text = t
		elseif quest.status ~= "completed" and c == 1 then
			Gui.chat_history:append{text = "Completed quest: " .. quest.name}
			quest.status = "completed"
			quest.text = t
		elseif quest.text ~= t then
			Gui.chat_history:append{text = "Updated quest: " .. quest.name}
			quest.text = t
		end
	end
end}

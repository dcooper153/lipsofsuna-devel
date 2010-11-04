Quests = Class()
Quests.dict_name = {}

Quests.init = function(clss)
	local dialog = Widget{cols = 2, rows = 1}
	dialog.list = Widgets.List()
	dialog.list.pressed = function(view, row) Quests:show(row) end
	dialog.list:set_request{width = 100, height = 100}
	dialog.quest_info = Widgets.QuestInfo()
	dialog.title_label = Widgets.Label{font = "medium", text = "Quests"}
	dialog.group1 = Widget{cols = 1, rows = 2}
	dialog.group1:set_expand{col = 1, row = 2}
	dialog.group1:set_child{col = 1, row = 1, widget = dialog.title_label}
	dialog.group1:set_child{col = 1, row = 2, widget = dialog.list}
	dialog.spacings = {5, 5}
	dialog:set_expand{col = 2, row = 1}
	dialog:set_child{col = 1, row = 1, widget = dialog.group1}
	dialog:set_child{col = 2, row = 1, widget = dialog.quest_info}
	clss.window = dialog
end

--- Shows a quest.
-- @param clss Quests class.
-- @param name Quest name.
Quests.show = function(clss, name)
	local quest = Quest:find{name = name}
	if not quest then return end
	clss.window.quest_info.quest = quest
end

--- Updates a quest.
-- @param clss Quests class.
-- @param quest Quest.
Quests.update = function(clss, quest)
	local button = clss.dict_name[quest.name]
	if not button then
		button = Widgets.Button{
			pressed = function(self) Quests:show(self.text) end,
			text = quest.name}
		clss.window.list:append{widget = button}
		clss.dict_name[quest.name] = button
	end
	clss:show(quest.name)
end

------------------------------------------------------------------------------

Quests:init()

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
			Quests:update(quest)
		elseif quest.status ~= "completed" and c == 1 then
			Gui.chat_history:append{text = "Completed quest: " .. quest.name}
			quest.status = "completed"
			quest.text = t
			Quests:update(quest)
		elseif quest.text ~= t then
			Gui.chat_history:append{text = "Updated quest: " .. quest.name}
			quest.text = t
			Quests:update(quest)
		end
		Effect:play("quest1")
	end
end}

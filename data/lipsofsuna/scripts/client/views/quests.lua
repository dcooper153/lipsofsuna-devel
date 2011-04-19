Views.Quests = Class(Widget)

Views.Quests.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 3, spacings = {0, 0}})
	self.dict_name = {}
	self.list = Widgets.List()
	self.list.pressed = function(view, row) self:show(row) end
	self.quest_info = Widgets.QuestInfo()
	self.title = Widgets.Frame{style = "title", text = "Quest"}
	self.frame = Widgets.Frame{style = "default", cols = 1, rows = 1}
	self.frame:set_child{col = 1, row = 1, widget = self.list}
	self.frame:set_expand{col = 1, row = 1}
	self:set_expand{col = 1, row = 2}
	self:set_child{col = 1, row = 1, widget = self.title}
	self:set_child{col = 1, row = 2, widget = self.frame}
	self:set_child{col = 1, row = 3, widget = self.quest_info}
	return self
end

Views.Quests.back = function(self)
	Gui:set_mode("menu")
end

--- Gets the compass direction for the currently shown quest.<br/>
-- The returned direction is in radians. The coordinate convention is:<ul>
--  <li>north: 0*pi rad</li>
--  <li>east: 0.5*pi rad</li>
--  <li>south: 1*pi rad</li>
--  <li>west: 1.5*pi rad</li></ul>
-- @param self Quest class.
-- @return Compass direction in radians, or nil.
Views.Quests.get_compass_direction = function(self)
	if self.shown_quest then
		local quest = Quest:find{name = self.shown_quest}
		if not quest or not quest.marker then return end
		if not Player.object then return end
		local diff = quest.marker - Player.object.position
		return 0.5 * math.pi + math.atan2(diff.z, -diff.x)
	end
end

--- Shows a quest.
-- @param self Quests class.
-- @param name Quest name.
Views.Quests.show = function(self, name)
	local quest = Quest:find{name = name}
	if not quest then return end
	self.quest_info.quest = quest
	self.shown_quest = name
end

--- Updates a quest.
-- @param self Quests class.
-- @param quest Quest.
Views.Quests.update = function(self, quest)
	local button = self.dict_name[quest.name]
	if not button then
		button = Widgets.Button{
			pressed = function(widget) self:show(widget.text) end,
			text = quest.name}
		self.list:append{widget = button}
		self.dict_name[quest.name] = button
	end
	self:show(quest.name)
end

------------------------------------------------------------------------------

Views.Quests.inst = Views.Quests()

-- Sets to position of the quest marker.
Protocol:add_handler{type = "QUEST_MARKER", func = function(event)
	local ok,id,x,y,z = event.packet:read("uint32", "float", "float", "float")
	if ok then
		local quest = Quest:find{id = id}
		if not quest then return end
		if x > 0.1 and y > 0.1 and z > 0.1 then
			quest.marker = Vector(x,y,z)
		else
			quest.marker = nil
		end
	end
end}

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
			Views.Quests.inst:update(quest)
		elseif quest.status ~= "completed" and c == 1 then
			Gui.chat_history:append{text = "Completed quest: " .. quest.name}
			quest.status = "completed"
			quest.text = t
			Views.Quests.inst:update(quest)
		elseif quest.text ~= t then
			Gui.chat_history:append{text = "Updated quest: " .. quest.name}
			quest.text = t
			Views.Quests.inst:update(quest)
		end
		Effect:play("quest1")
	end
end}

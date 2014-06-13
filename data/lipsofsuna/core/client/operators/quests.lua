local Class = require("system/class")
local QuestSpec = require("core/specs/quest")

Operators.quests = Class("QuestsOperator")
Operators.quests.data = {}

--- Resets the quest manager.</br>
--
-- Context: Any.
--
-- @param self Operator.
Operators.quests.reset = function(self)
	self.data = {sound_timer = Program:get_time(), quests = {}}
end

--- Adds a new quest.<br/>
--
-- Context: Any.
--
-- @param self Client.
-- @param quest Quest information.
Operators.quests.add_quest = function(self, quest)
	-- Add to the dictionary.
	self.data.quests[quest.name] = quest
	-- Set as active if nothing else is active.
	if not self.data.shown and quest.status ~= "completed" then
		self.data.shown = quest
	end
end

--- Gets the currently shown map marker.</br>
--
-- Context: Any.
--
-- @param self Client.
-- @return Marker, or nil.
Operators.quests.get_active_marker = function(self)
	if not self.data.shown then return end
	local quest = self.data.shown
	if not quest or not quest.marker then return end
	return Main.markers:find_by_name(quest.marker)
end

--- Gets the active quest.</br>
--
-- Context: Any.
--
-- @param self Client.
-- @return Quest, or nil.
Operators.quests.get_active_quest = function(self)
	return self.data.shown
end

--- Sets the active quest.</br>
--
-- Context: Any.
--
-- @param self Client.
-- @param quest Quest, or nil.
Operators.quests.set_active_quest = function(self, quest)
	self.data.shown = quest
end

--- Gets the compass direction for the currently shown quest.<br/>
--
-- Context: Any.<br/>
--
-- The returned direction is in radians. The coordinate convention is:<ul>
--  <li>north: 0*pi rad</li>
--  <li>east: 0.5*pi rad</li>
--  <li>south: 1*pi rad</li>
--  <li>west: 1.5*pi rad</li></ul>
--
-- @param self Client.
-- @return Compass direction in radians, or nil.
Operators.quests.get_compass_direction = function(self)
	-- Find the marker.
	local marker = self:get_active_marker()
	if not marker then return end
	-- Calculate the direction.
	if not Client.player_object then return end
	local diff = marker.position - Client.player_object:get_position()
	return 0.5 * math.pi + math.atan2(diff.z, -diff.x)
end

--- Gets the distance to the shown quest marker in the X-Z plane.<br/>
--
-- Context: Any.
--
-- @param self Client.
-- @return Compass distance, or nil.
Operators.quests.get_compass_distance = function(self)
	-- Find the marker.
	local marker = self:get_active_marker()
	if not marker then return end
	-- Calculate the distance.
	if not Client.player_object then return end
	local diff = marker.position - Client.player_object:get_position()
	diff.y = 0
	return diff.length
end

--- Gets the compass height offset for the currently shown quest.<br/>
--
-- Context: Any.
--
-- @param self Client.
-- @return Compass height offset, or nil.
Operators.quests.get_compass_height = function(self)
	-- Find the marker.
	local marker = self:get_active_marker()
	if not marker then return end
	-- Calculate the height offset.
	if not Client.player_object then return end
	local diff = marker.position - Client.player_object:get_position()
	return diff.y
end

--- Gets quest status by quest ID.<br/>
--
-- Context: Any.
--
-- @param self Client.
-- @return Quest status, or nil.
Operators.quests.get_quest_by_name = function(self, name)
	return self.data.quests[name]
end

--- Set the status of the quest.<br/>
--
-- Context: Any.
--
-- @param self Client.
-- @param quest Quest.
-- @param status New status.
-- @param text Quest text.
Operators.quests.set_quest_status = function(self, quest, status, text)
	local t = Program:get_time()
	local joining = Operators.play:is_startup_period()
	local silent = joining or (t - self.data.sound_timer < 2)
	-- Update the status.
	if quest.status == "inactive" and status == "active" then
		if not joining then Client:append_log("Started quest: " .. quest.spec.name) end
		quest.status = "active"
		quest.text = text
	elseif quest.status == "completed" and status == "active" then
		if not joining then Client:append_log("Restarted quest: " .. quest.spec.name) end
		quest.status = "active"
		quest.text = text
	elseif quest.status ~= "completed" and status == "completed" then
		if not joining then Client:append_log("Completed quest: " .. quest.spec.name) end
		quest.status = "completed"
		quest.text = text
	elseif quest.text ~= text then
		if not joining then Client:append_log("Updated quest: " .. quest.spec.name) end
		quest.text = text
	else
		return
	end
	-- Play a sound effect unless not played too recently.
	if not silent then
		Client.data.sound_timer = t
		Client.effects:play("quest1")
	end
	-- Set as active if nothing else is active.
	if not self.data.shown and status ~= "completed" then
		self.data.shown = quest.name
	end
end

--- Returns a sorted list of quests.<br/>
--
-- Context: Any.
--
-- @param self Client.
-- @return List of quests.
Operators.quests.get_quests = function(self)
	local quests = {}
	for name,quest in pairs(self.data.quests) do
		local spec = QuestSpec:find_by_name(name)
		table.insert(quests, {spec, quest})
	end
	table.sort(quests, function(a, b) return a[1].name < b[1].name end)
	return quests
end

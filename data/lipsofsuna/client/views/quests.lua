Views.Quests = Class(Widget)

Views.Quests.new = function(clss)
	local self = Widget.new(clss)
	return self
end

Views.Quests.back = function(self)
end

Views.Quests.enter = function(self, from, level)
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
	-- Find the marker.
	local marker = self:get_active_marker()
	if not marker then return end
	-- Calculate the direction.
	local diff = marker.position - Player.object.position
	return 0.5 * math.pi + math.atan2(diff.z, -diff.x)
end

--- Gets the distance to the shown quest marker in the X-Z plane.
-- @param self Quest class.
-- @return Compass distance or nil.
Views.Quests.get_compass_distance = function(self)
	-- Find the marker.
	local marker = self:get_active_marker()
	if not marker then return end
	-- Calculate the distance.
	local diff = marker.position - Player.object.position
	diff.y = 0
	return diff.length
end

--- Gets the compass height offset for the currently shown quest.
-- @param self Quest class.
-- @return Compass height offset or nil.
Views.Quests.get_compass_height = function(self)
	-- Find the marker.
	local marker = self:get_active_marker()
	if not marker then return end
	-- Calculate the height offset.
	local diff = marker.position - Player.object.position
	return diff.y
end

--- Gets the currently shown map marker.
-- @param self Quests view.
-- @return Marker or nil.
Views.Quests.get_active_marker = function(self)
	if not self.shown_quest then return end
	if not Player.object then return end
	local quest = Quest:find{name = self.shown_quest}
	if not quest or not quest.marker then return end
	return Marker:find{name = quest.marker}
end

--- Shows a quest.
-- @param self Quests class.
-- @param name Quest name.
Views.Quests.show = function(self, name)
	self.shown_quest = name
end

--- Updates a quest.
-- @param self Quests class.
-- @param quest Quest.
Views.Quests.update = function(self, quest)
	self:show(quest.name)
end

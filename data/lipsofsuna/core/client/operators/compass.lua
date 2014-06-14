local QuestSpec = require("core/specs/quest")

--- Gets the currently shown map marker.
-- @param self Client.
-- @return Marker or nil.
Client.get_active_marker = function(self)
	if not self.data.quest.shown then return end
	if not self.player_object then return end
	local quest = Main.specs:find_by_name("QuestSpec", self.data.quest.shown)
	if not quest or not quest.marker then return end
	return Main.markers:find_by_name(quest.marker)
end

--- Gets the compass direction for the currently shown quest.<br/>
-- The returned direction is in radians. The coordinate convention is:<ul>
--  <li>north: 0*pi rad</li>
--  <li>east: 0.5*pi rad</li>
--  <li>south: 1*pi rad</li>
--  <li>west: 1.5*pi rad</li></ul>
-- @param self Client.
-- @return Compass direction in radians, or nil.
Client.get_compass_direction = function(self)
	-- Find the marker.
	local marker = self:get_active_marker()
	if not marker then return end
	-- Calculate the direction.
	local diff = marker.position - self.player_object:get_position()
	return 0.5 * math.pi + math.atan2(diff.z, -diff.x)
end

--- Gets the distance to the shown quest marker in the X-Z plane.
-- @param self Client.
-- @return Compass distance, or nil.
Client.get_compass_distance = function(self)
	-- Find the marker.
	local marker = self:get_active_marker()
	if not marker then return end
	-- Calculate the distance.
	local diff = marker.position - self.player_object:get_position()
	diff.y = 0
	return diff.length
end

--- Gets the compass height offset for the currently shown quest.
-- @param self Client.
-- @return Compass height offset, or nil.
Client.get_compass_height = function(self)
	-- Find the marker.
	local marker = self:get_active_marker()
	if not marker then return end
	-- Calculate the height offset.
	local diff = marker.position - self.player_object:get_position()
	return diff.y
end

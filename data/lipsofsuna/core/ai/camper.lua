--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.ai.camper
-- @alias CamperAi

local Class = require("system/class")
local NpcAi = require(Mod.path .. "npc")

--- TODO:doc
-- @type CamperAi
local CamperAi = Class("CamperAi", NpcAi)
CamperAi.type = "camper"

--- Creates a new camper AI.
-- @param clss CamperAi class.
-- @param object Controlled actor.
-- @return AI.
CamperAi.new = function(clss, object)
	local self = NpcAi.new(clss, object)
	return self
end

--- Analyzes the surrounding terrain and chooses a wander target.
-- @param self AI.
CamperAi.choose_wander_target = function(self)
	local home = self.object.home_point
	if home then
		local dist = (self.object:get_position() - self.object.home_point).length
		if dist > 15 * math.random() then
			self.target = home
			if self:avoid_wander_obstacles(home) then return end
		end
	end
	NpcAi.choose_wander_target(self)
end

--- Updates the AI.
-- @param self AI.
-- @param secs Seconds since the last update.
CamperAi.update = function(self, secs)
	-- Store the home point.
	if not self.object.home_point then
		self.object.home_point = self.object:get_position()
	end
	-- Update the base AI.
	NpcAi.update(self, secs)
end

return CamperAi



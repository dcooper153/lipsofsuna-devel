--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.ai.anchored
-- @alias AnchoredAi

local Class = require("system/class")
local CamperAi = require(Mod.path .. "camper")

--- TODO:doc
-- @type AnchoredAi
local AnchoredAi = Class("AnchoredAi", CamperAi)
AnchoredAi.type = "anchored"

--- Creates a new anchored AI.
-- @param clss CamperAi class.
-- @param object Controlled actor.
-- @return AI.
AnchoredAi.new = function(clss, object)
	local self = CamperAi.new(clss, object)
	return self
end

--- Analyzes the surrounding terrain and chooses a wander target.
-- @param self AI.
AnchoredAi.choose_wander_target = function(self)
	local home = self.object.home_point
	if home then
		self.target = home
		self:avoid_wander_obstacles(home)
	end
end

return AnchoredAi



--- Companion AI.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.ai.logic.companion
-- @alias CompanionAi

local Class = require("system/class")
local NpcAi = require("core/ai/logic/npc")

--- Companion AI.
-- @type CompanionAi
local CompanionAi = Class("CompanionAi", NpcAi)
CompanionAi.type = "companion"

--- Creates a new companion AI.
-- @param clss AI class.
-- @param manager AI manager.
-- @param object Controlled actor.
-- @return AI.
CompanionAi.new = function(clss, manager, object)
	local self = NpcAi.new(clss, manager, object)
	return self
end

--- Analyzes the surrounding terrain and chooses a wander target.
-- @param self AI.
CompanionAi.choose_wander_target = function(self)
	if self.object.owner and self.object.owner:get_visible() then
		local pos = self.object:get_position() 
		local target = self.object.owner:get_position() 
		local dist = (pos - target).length
		if dist > 2 then
			self.target = target:copy()
		else
			self.target = pos:copy()
		end
	end
	NpcAi.choose_wander_target(self)
end

return CompanionAi

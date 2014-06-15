--- Manages the AI of the game.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.ai.ai_manager
-- @alias AiManager

local Class = require("system/class")

--- Manages the AI of the game.
-- @type AiManager
local AiManager = Class("AiManager")

--- Creates a new AI manager.
-- @return AiManager.
AiManager.new = function(clss)
	local self = Class.new(clss)
	self.__classes = {}
	self.logics = setmetatable({}, {__mode = "k"})
	return self
end

--- Adds an AI to the update list.
-- @param self AiManager.
-- @param ai AI.
AiManager.add = function(self, ai)
	self.logics[ai] = true
end

--- Removes an AI from the update list.
-- @param self AiManager.
-- @param ai AI.
AiManager.remove = function(self, ai)
	self.logics[ai] = nil
end

--- Finds an AI logic class.
-- @param self AiManager.
-- @param type Logic type.
-- @return Logic class on success. Nil otherwise.
AiManager.find_class_by_type = function(self, type)
	return self.__classes[type]
end

--- Registers an AI logic class.
-- @param self AiManager.
-- @param logic Logic class.
AiManager.register_class = function(self, logic)
	self.__classes[logic.type] = logic
end

--- Updates the AI of all the objects.
-- @param self AiManager.
-- @param secs Seconds sincle the last update.
AiManager.update = function(self, secs)
	for k,v in pairs(self.logics) do
		if k.object and k.object:get_visible() then
			k:update(secs)
		end
	end
end

return AiManager

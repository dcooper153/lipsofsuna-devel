--- Manages game modes provided by mods.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module main.game_mode_manager
-- @alias GameModeManager

local Class = require("system/class")

------------------------------------------------------------------------------

--- Manages game modes provided by mods.
-- @type GameModeManager
local GameModeManager = Class("GameModeManager")

--- Creates a new game mode manager.
-- @param clss GameModeManager class.
-- @return New hook manager.
GameModeManager.new = function(clss)
	local self = Class.new(clss)
	self.__modes = {}
	return self
end

--- Registers a game mode hook.
-- @param self GameModeManager.
-- @param name Mode name.
-- @param hook Initialization function.
GameModeManager.register = function(self, name, hook)
	self.__modes[name] = hook
end

--- Starts the given game mode.
-- @param self GameModeManager.
-- @param name Mode name.
-- @return True if found. False otherwise.
GameModeManager.start = function(self, name)
	local mode = self.__modes[name]
	if not mode then return end
	mode()
	return true
end

--- Gets the game mode list.
-- @param self GameModeManager.
-- @return Iterator.
GameModeManager.get_modes = function(self)
	return coroutine.wrap(function()
		for k,v in pairs(self.__modes) do
			coroutine.yield(k, v)
		end
	end)
end

return GameModeManager

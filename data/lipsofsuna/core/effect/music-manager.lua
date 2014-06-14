--- Manages music playback.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.effect.music_manager
-- @alias MusicManager

local Class = require("system/class")
local Client = require("core/client/client")
local Sound = require("system/sound")

--- Manages music playback.
-- @type MusicManager
local MusicManager = Class("MusicManager")

--- Creates the music manager.
-- @param clss MusicManager class.
-- @return MusicManager.
MusicManager.new = function(clss)
	local self = Class.new(clss)
	self.__combat_hints = setmetatable({}, {__mode = "k"})
	self.__combat_timer = nil
	self.__fade_out = 2
	self.__music_mode = "none"
	return self
end

--- Clears all the combat hints.
-- @param self MusicManager.
MusicManager.clear_combat_hints = function(self)
	self.__combat_hints = setmetatable({}, {__mode = "k"})
	self.__combat_timer = nil
end

--- Switches the music track.
-- @param self MusicManager.
-- @param force True to force even in combat. Nil otherwise.
MusicManager.cycle_track = function(self, force)
	-- Check for combat.
	if self.__combat_timer and not force then return end
	local mode = self.__combat_timer and "combat" or self.__music_mode
	if not mode then return end
	-- Find the tracks.
	local tracks = Main.specs:find_by_category("MusicSpec", mode)
	if not tracks then return end
	if #tracks == 0 then return end
	-- Choose a random track.
	local spec = tracks[math.random(1, #tracks)]
	-- Crossfade with the previous track.
	Sound:set_music_fading(math.max(spec.fade_in, self.__fade_out))
	Sound:set_music_volume(spec.volume * Client.options.music_volume)
	Sound:set_music(spec.file)
	self.__fade_out = spec.fade_out
end

--- Switches the music track.
-- @param self MusicManager.
-- @param mode Music mode.
-- @param force True to force even if combat or the mode is the same. Nil otherwise.
MusicManager.switch_track = function(self, mode, force)
	-- Switch the music mode.
	if not mode then return end
	if not force and mode == self.__music_mode then return end
	self.__music_mode = mode
	-- Don't switch the mode during combat.
	if not force and self.__combat_timer then return end
	-- Select a track for the new mode.
	self:cycle_track()
end

--- Updates the music playback.
-- @param self MusicManager.
-- @param secs Seconds since the last update.
MusicManager.update = function(self, secs)
	-- Check if we are is in combat.
	local combat
	for k,v in pairs(self.__combat_hints) do
		combat = true
		break
	end
	-- Update the combat track.
	if combat then
		local prev = self.__combat_timer
		self.__combat_timer = 10
		if not prev then
			self:cycle_track(true)
		end
	elseif self.__combat_timer then
		self.__combat_timer = self.__combat_timer - secs
		if self.__combat_timer <= 0 then
			self.__combat_timer = nil
			self:switch_track("game", true)
		end
	end
end

--- Informs the manager about combat changes.
-- @param self MusicManager.
-- @param object Object whose combat status has changer.
-- @param value True for combat. False otherwise.
MusicManager.set_combat_hint = function(self, object, value)
	-- Toggle the hint for the object.
	if value then
		self.__combat_hints[object] = true
	else
		self.__combat_hints[object] = nil
	end
end

return MusicManager

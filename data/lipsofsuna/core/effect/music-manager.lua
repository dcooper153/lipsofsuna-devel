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
	self.__music_mode = "none"
	self.__combat_hints = setmetatable({}, {__mode = "k"})
	self.__combat_timer = nil
	return self
end

--- Switches the music track.
-- @param self EffectManager.
-- @param force True to force even in combat. Nil otherwise.
MusicManager.cycle_track = function(self, force)
	local modes = {
		boss = {"fairytale7", "fairytale9"},
		char = {"fairytale2", "fairytale10", "fairytale8"},
		combat = {"xeon5"},
		game = {"fairytale1", "fairytale3", "fairytale4", "fairytale5",
		        "fairytale6", "fairytale11", "fairytale12", "fairytale13"},
		intro = {"fairytale7"},
		menu = {"menu1"}}
	-- Check for combat.
	if self.__combat_timer and not force then return end
	local mode = self.__combat_timer and "combat" or self.__music_mode
	if not mode then return end
	-- Find the tracks.
	local tracks = modes[mode]
	if not tracks then return end
	-- Crossfade find a random track.
	Sound:set_music_fading(2.0)
	Sound:set_music_volume(Client.options.music_volume)
	Sound:set_music(tracks[math.random(1, #tracks)])
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

--- The main loop of the game.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module main.main
-- @alias Main

require("main/event")
local Class = require("system/class")
local Eventhandler = require("system/eventhandler")
local Hooks = require("system/hooks")
local GameModeManager = require("main/game-mode-manager")
local ImageManager = require("main/image-manager")
local Log = require("main/log")
local Messaging = require("main/messaging")
local ModelManager = require("main/model-manager")
local Mod = require("main/mod")
local Physics = require("system/physics")
local Settings = require("main/settings")
local Timing = require("main/timing")
local Watchdog = require("system/watchdog")

--- The main loop of the game.
-- @type Main
local Main = Class("Main")

--- Creates and enters the main loop.
-- @param clss Main class.
Main.new = function(clss)
	local self = Class.new(clss)
	self.log = Log()
	self.images = ImageManager()
	self.models = ModelManager()
	self.timing = Timing()
	self.mods = Mod()
	self.settings = Settings(self.mods)
	self.messaging = Messaging()
	self.game_modes = GameModeManager()
	self.game_start_hooks = Hooks()
	self.game_end_hooks = Hooks()
	self.game_load_hooks = Hooks()
	self.game_save_hooks = Hooks()
	self.main_start_hooks = Hooks()
	self.main_end_hooks = Hooks()
	self.update_hooks = Hooks()
	_G["Mod"] = self.mods --FIXME: global
	_G["Settings"] = self.settings --FIXME: global
	return self
end

--- Loads the mods and their data.
-- @param self Main.
Main.load = function(self)
	-- Load the mods.
	self.mods:load("main")
	self.mods:load_list("mods.json")
	-- Handle command line arguments.
	if not self.settings:parse_command_line() then return end
	self.start = true
	-- Initialize the mods.
	self.mods:init_all(self.settings)
end

--- Enters the main loop.
-- @param self Main.
Main.main = function(self)
	-- Check for early exit.
	if not self.start then
		print(self.settings:usage())
		return
	end
	if self.settings.quit then
		Program:set_quit(true)
	end
	-- Start the game.
	self.main_start_hooks:call()
	local mode = self.settings:get_game_mode()
	if mode then
		self:start_game(mode)
	else
		self.client:show_main_menu(self.settings)
	end
	-- Enter the main loop.
	self:enable_manual_gc()
	while not Program:get_quit() do
		-- Update the program state.
		local tick = self.timing:get_frame_duration()
		self.timing:start_frame()
		self.timing:start_action("program")
		if self.settings.watchdog then
			Watchdog:start(30)
		end
		Program:update()
		-- Update physics.
		self.timing:start_action("physics")
		Physics:update(tick)
		-- Update the scene.
		if self.client then
			self.timing:start_action("scene")
			Program:update_scene(tick)
		end
		-- Process events.
		self.timing:start_action("event")
		Eventhandler:update()
		-- Update the logic.
		self.timing:start_action("objects")
		if self.objects then
			self.objects:update(tick)
		end
		self.timing:start_action("hooks")
		self.update_hooks:call(tick)
		self.timing:start_action("resources")
		self.images:update(tick)
		self.models:update(tick)
		-- Render the scene.
		if self.client then
			self.timing:start_action("render")
			Program:render_scene()
		end
		-- Collect garbage.
		self.timing:start_action("garbage")
		self:perform_manual_gc(tick)
	end
	self.main_end_hooks:call()
end

--- Ends the game.
-- @param self Main.
Main.end_game = function(self)
	if not self.game then return end
	self.game:free()
	self.game = nil
	self.messaging:set_transmit_mode(false, false, nil)
	collectgarbage()
end

--- Starts the game.
-- @param self Main.
-- @param mode Game mode, "benchmark"/"editor"/"host"/"join"/"server".
-- @param save Save file name, or nil when not using local I/O.
-- @param port Server port number, or nil if not hosting.
Main.start_game = function(self, mode, save, port)
	-- Call the mode start function.
	self.game_modes:start(mode)
end

--- Enables manual garbage collection.
-- @param self Main.
Main.enable_manual_gc = function(self)
	-- Stop automatic collection from now on.
	collectgarbage("stop")
	collectgarbage("setpause", 1000)
	collectgarbage("setstepmul", 10)
	-- Perform full garbage collection.
	local memory0 = collectgarbage("count")
	local time0 = Program:get_time()
	collectgarbage()
	local time1 = Program:get_time()
	local memory1 = collectgarbage("count")
	-- Estimate the garbage collection rate.
	self.__need_memory = memory1
	self.__collect_time = 0.001
	self.__collect_rate = (memory0 - memory1) / (time1 - time0)
	self.__collect_thresh = memory1 * 1.5
end

--- Collects garbage manually.
-- @param self Main.
-- @param secs Seconds since the last collection.
Main.perform_manual_gc = function(self, secs)
	-- Estimate the needed collection time.
	local rate = self.__collect_rate
	local thresh = self.__collect_thresh
	local memory = collectgarbage("count")
	local time = math.max(0, memory - thresh) / (rate * secs)
	time = math.min(time, self.__collect_time + 0.0001)
	self.__collect_time = time
	-- Collect garbage.
	Program:collect_garbage(time)
	collectgarbage("stop")
	-- Estimate the collection rate.
	local collected = memory - collectgarbage("count")
	if collected > 0 and time > 0 then
		self.__collect_rate = collected / time
	end
	-- TODO: Estimate the real memory consumption.
	-- self.__need_memory = ???
	-- self.__collect_thresh = self.__need_memory * ???
end

return Main()

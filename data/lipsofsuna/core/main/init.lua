--- The main loop of the game.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.main.init
-- @alias Main

require("core/main/event")
local Class = require("system/class")
local Eventhandler = require("system/eventhandler")
local ModelManager = require("core/main/model-manager")
local Mod = require("core/main/mod")
local Settings = require("core/main/settings")
local Timing = require("core/main/timing")
local Watchdog = require("system/watchdog")

--- The main loop of the game.
-- @type Main
local Main = Class("Main")

--- Creates and enters the main loop.
-- @param clss Main class.
Main.new = function(clss)
	local self = Class.new(clss)
	self.models = ModelManager()
	self.timing = Timing()
	self.mods = Mod()
	self.settings = Settings(self.mods)
	_G["Mod"] = self.mods --FIXME: global
	_G["Settings"] = self.settings --FIXME: global
	return self
end

--- Loads the mods and their data.
-- @param self Main.
Main.load = function(self)
	-- Load the mods.
	self.mods:load_list("mods.json")
	-- Handle command line arguments.
	if not self.settings:parse_command_line() then return end
	self.start = true
	-- Initialize the mods.
	self.mods:init_all(self.settings)
end

--- Enters the main loop.
-- @param self Main class.
Main.main = function(self)
	if not self.start then
		print(self.settings:usage())
		return
	end
	-- Validate specs.
	Actorspec:validate_all()
	Actorpresetspec:validate_all()
	Animationspec:validate_all()
	AnimationProfileSpec:validate_all()
	CraftingRecipeSpec:validate_all()
	Dialogspec:validate_all()
	Factionspec:validate_all()
	Feattypespec:validate_all()
	Feateffectspec:validate_all()
	Iconspec:validate_all()
	Itemspec:validate_all()
	Obstaclespec:validate_all()
	Patternspec:validate_all()
	Questspec:validate_all()
	Skillspec:validate_all()
	Spellspec:validate_all()
	Staticspec:validate_all()
	-- Enter the main loop.
	if self.settings.quit then
		Program:set_quit(true)
	elseif self.settings.server then
		-- Server main.
		Game:init("server", self.settings.file, self.settings.port)
		Server:load()
		Program:set_sleep(1/60)
		self:enable_manual_gc()
		while not Program:get_quit() do
			-- Update the program state.
			local tick = self.timing:get_frame_duration()
			self.timing:start_frame()
			self.timing:start_action("program")
			Program:update()
			-- Process events.
			self.timing:start_action("event")
			Eventhandler:update()
			-- Update the logic.
			self.timing:start_action("sectors")
			if Game.initialized then
				Game.sectors:update(tick)
			end
			self.timing:start_action("terrain")
			if Game.initialized then
				Game.terrain:update(tick)
			end
			self.timing:start_action("server")
			Server:update(tick)
			self.timing:start_action("models")
			self.models:update(tick)
			-- Collect garbage.
			self.timing:start_action("garbage")
			self:perform_manual_gc(tick)
		end
		Game:deinit()
	else
		-- Client main.
		Client:init()
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
			Program:update_scene(tick)
			-- Process events.
			self.timing:start_action("event")
			Eventhandler:update()
			-- Update the logic.
			self.timing:start_action("sectors")
			if Game.initialized then
				Game.sectors:update(tick)
			end
			self.timing:start_action("terrain")
			if Game.initialized then
				Game.terrain:update(tick)
			end
			self.timing:start_action("server")
			Server:update(tick)
			self.timing:start_action("client")
			Client:update(tick)
			self.timing:start_action("models")
			self.models:update(tick)
			-- Render the scene.
			self.timing:start_action("render")
			Program:render_scene()
			-- Collect garbage.
			self.timing:start_action("garbage")
			self:perform_manual_gc(tick)
		end
		Client:deinit()
	end
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
	manualgcenable=true
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

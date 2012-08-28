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
local Timing = require("core/main/timing")

--- The main loop of the game.
-- @type Main
Main = Class("Main")

--- Creates and enters the main loop.
-- @param clss Main class.
Main.new = function(clss)
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
	-- Initialize.
	clss.models = ModelManager()
	clss.timing = Timing()
	-- Enter the main loop.
	if Settings.quit then
		Program:set_quit(true)
	elseif Settings.server then
		-- Server main.
		Game:init("server", Settings.file, Settings.port)
		Server:load()
		Program:set_sleep(1/60)
		clss:enable_manual_gc()
		while not Program:get_quit() do
			-- Update the program state.
			local tick = clss.timing:get_frame_duration()
			clss.timing:start_frame()
			clss.timing:start_action("program")
			Program:update()
			-- Process events.
			clss.timing:start_action("event")
			Eventhandler:update()
			-- Update the logic.
			clss.timing:start_action("sectors")
			if Game.initialized then
				Game.sectors:update(tick)
			end
			clss.timing:start_action("server")
			Server:update(tick)
			clss.timing:start_action("models")
			clss.models:update(tick)
			-- Collect garbage.
			clss.timing:start_action("garbage")
			clss:perform_manual_gc()
		end
		Game:deinit()
	else
		-- Client main.
		Client:init()
		clss:enable_manual_gc()
		while not Program:get_quit() do
			-- Update the program state.
			local tick = clss.timing:get_frame_duration()
			clss.timing:start_frame()
			clss.timing:start_action("program")
			if Settings.watchdog then
				Program:watchdog_start(30)
			end
			Program:update()
			Program:update_scene(tick)
			-- Process events.
			clss.timing:start_action("event")
			Eventhandler:update()
			-- Update the logic.
			clss.timing:start_action("sectors")
			if Game.initialized then
				Game.sectors:update(tick)
			end
			clss.timing:start_action("server")
			Server:update(tick)
			clss.timing:start_action("client")
			Client:update(tick)
			clss.timing:start_action("models")
			clss.models:update(tick)
			-- Render the scene.
			clss.timing:start_action("render")
			Program:render_scene()
			-- Collect garbage.
			clss.timing:start_action("garbage")
			clss:perform_manual_gc(tick)
		end
		Client:deinit()
	end
end

--- Enables manual garbage collection.
-- @param clss Main class.
Main.enable_manual_gc = function(clss)
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
	clss.__need_memory = memory1
	clss.__collect_time = 0.001
	clss.__collect_rate = (memory0 - memory1) / (time1 - time0)
	clss.__collect_thresh = memory1 * 1.5
	manualgcenable=true
end

--- Collects garbage manually.
-- @param clss Main class.
-- @param secs Seconds since the last collection.
Main.perform_manual_gc = function(clss, secs)
	-- Estimate the needed collection time.
	local rate = clss.__collect_rate
	local thresh = clss.__collect_thresh
	local memory = collectgarbage("count")
	local time = math.max(0, memory - thresh) / (rate * secs)
	time = math.min(time, clss.__collect_time + 0.0001)
	clss.__collect_time = time
	-- Collect garbage.
	Program:collect_garbage(time)
	collectgarbage("stop")
	-- Estimate the collection rate.
	local collected = memory - collectgarbage("count")
	if collected > 0 and time > 0 then
		clss.__collect_rate = collected / time
	end
	-- TODO: Estimate the real memory consumption.
	-- clss.__need_memory = ???
	-- clss.__collect_thresh = clss.__need_memory * ???
end

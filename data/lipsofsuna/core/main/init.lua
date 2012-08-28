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
		end
		Game:deinit()
	else
		-- Client main.
		Client:init()
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
		end
		Client:deinit()
	end
end

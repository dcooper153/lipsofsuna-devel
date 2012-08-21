require(Mod.path .. "event")
local Class = require("system/class")
local Eventhandler = require("system/eventhandler")
local ModelManager = require(Mod.path .. "model-manager")

Main = Class("Main")

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
	-- Enter the main loop.
	if Settings.quit then
		Program:set_quit(true)
	elseif Settings.server then
		-- Server main.
		Game:init("server", Settings.file, Settings.port)
		Server:load()
		Program:set_sleep(1/60)
		Program.profiling = {}
		local frame = Program:get_time()
		while not Program:get_quit() do
			-- Update program state.
			local t1 = Program:get_time()
			Program:update()
			local t2 = Program:get_time()
			Eventhandler:update()
			Server:update(t1 - frame)
			clss.models:update(t1 - frame)
			local t3 = Program:get_time()
			-- Store timings.
			Program.profiling.update = t2 - t1
			Program.profiling.event = t3 - t2
			frame = t1
		end
		Game:deinit()
	else
		-- Client main.
		Client:init()
		Program.profiling = {}
		local frame = Program:get_time()
		while not Program:get_quit() do
			-- Update program state.
			if Settings.watchdog then
				Program:watchdog_start(30)
			end
			local t1 = Program:get_time()
			Program:update()
			Program:update_scene(t1 - frame)
			local t2 = Program:get_time()
			Eventhandler:update()
			Server:update(t1 - frame)
			Client:update(t1 - frame)
			clss.models:update(t1 - frame)
			local t3 = Program:get_time()
			-- Render the scene.
			Program:render_scene()
			local t4 = Program:get_time()
			-- Update profiling stats.
			Program.profiling.update = t2 - t1
			Program.profiling.event = t3 - t2
			Program.profiling.render = t4 - t3
			frame = t1
		end
		Client:deinit()
	end
end

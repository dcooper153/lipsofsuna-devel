if not Settings then return end
if Settings.server then return end

-- Initialize the default video mode.
local Options = require(Mod.path .. "options")
local options = Options()
__initial_videomode = {options.window_width, options.window_height, options.fullscreen, options.vsync, options.multisamples}
__initial_pointer_grab = false

require "system/graphics"
require "system/model-editing"
require "system/widgets"

Program:set_window_title("Lips of Suna")

require("core/client/bindings")
local Client = require("core/client/client")
local Network = require("system/network")
local Simulation = require("core/client/simulation")

-- FIXME: Most of these should be registered elsewhere.
Client:register_update_hook(10, function(secs)
	-- Update the simulation.
	Simulation:update(secs)
	-- Update the benchmark.
	if Client.benchmark then
		Client.benchmark:update(secs)
		return
	end
	-- Update the connection status.
	if Client:get_connected() and not Network:get_connected() then
		Client:terminate_game()
		Client.options.host_restart = false
		Client.data.connection.active = false
		Client.data.connection.waiting = false
		Client.data.connection.connecting = false
		Client.data.connection.text = "Lost connection to the server!"
		Client.data.load.next_state = "start-game"
		Ui:set_state("load")
	end
	-- Update the player state.
	Client.player_state:update(secs)
end)
Client:register_update_hook(20, function(secs)
	if Client.player_object then
		-- Refresh the active portion of the map.
		Client.player_object:refresh()
		-- Notify the terrain manager of the view center.
		Game.terrain:set_view_center(Client.player_object:get_position())
	end
	-- Update effects.
	-- Must be done after objects to ensure correct anchoring.
	if Game.initialized then
		for k in pairs(Game.scene_nodes_by_ref) do
			k:update(secs)
		end
	end
	-- FIXME
	if Client.player_object then
		Client.lighting:set_dungeon_mode(false)
	end
end)

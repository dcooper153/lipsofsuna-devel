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
local ClientRenderObject = require("core/client/client-render-object")
local MovementPrediction = require("core/client/movement-prediction")
local Network = require("system/network")
local Simulation = require("core/client/simulation")

Main.game_modes:register("Normal", function()
	Client:start_single_player()
end)

Main.main_start_hooks:register(10, function(secs)
	Main.objects.object_created_hooks:register(10, function(object)
		object.render = ClientRenderObject()
		if Main.game.enable_prediction then
			object.prediction = MovementPrediction()
		end
	end)
	Main.objects.object_detached_hooks:register(10, function(object)
		object.render:clear()
	end)
	Main.objects.object_update_hooks:register(10, function(object, secs)
		-- Interpolate the position.
		if object.prediction and object.prediction.enabled then
			object.prediction:update(secs)
			object:set_position(object.prediction:get_predicted_position())
			if object.dead or object ~= Client.player_object then
				object:set_rotation(object.prediction:get_predicted_rotation())
				object:set_tilt_angle(object.prediction:get_predicted_tilt())
			end
		end
		-- Update the render object.
		object.render:set_position(object:get_position())
		object.render:set_rotation(object:get_rotation())
	end)
	Main.client = Client --FIXME
	Main.client:init()
end)

Main.main_end_hooks:register(10, function(secs)
	Main.client:deinit()
end)

Main.update_hooks:register(10, function(secs)
	Main.timing:start_action("client")
	Main.client:update(secs)
end)

-- FIXME: Most of these should be registered elsewhere.
Client:register_update_hook(12, function(secs)
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
		Main.terrain:set_view_center(Client.player_object:get_position())
	end
	-- FIXME
	if Client.player_object then
		Client.lighting:set_dungeon_mode(false)
	end
end)

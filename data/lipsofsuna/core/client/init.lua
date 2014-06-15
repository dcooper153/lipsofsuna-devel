-- Initialize the default video mode.
local Options = require("core/client/options")
local options = Options()

__initial_videomode = {options.window_width, options.window_height, options.fullscreen, options.vsync, options.multisamples}
__initial_pointer_grab = false
require "system/graphics"
__initial_pointer_grab = nil
__initial_videomode = nil

require "system/model-editing"

Program:set_window_title("Lips of Suna")

require("core/client/bindings")
local Client = require("core/client/client")
local Network = require("system/network")

Main.game_modes:register("Normal", function()
	Client:start_single_player()
end)

Main.main_start_hooks:register(10, function(secs)
	Main.client = Client --FIXME
	Main.client:init()
end)

Main.main_start_hooks:register(200, function(secs)
	Main.client:reset_data()
	-- Execute the startup command.
	Main.client.options:apply()
	Main.client.start_hooks:call()
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

--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.client
-- @alias Client

local Class = require("system/class")
local File = require("system/file")
local Game = require("core/server/game")
local Hooks = require("system/hooks")
local Lighting = require("core/client/lighting")
local Options = require("core/client/options")
local Physics = require("system/physics")
local PlayerState = require("core/client/player-state")
local Reload = require("system/reload")
local Simulation = require("core/client/simulation")
local Skills = require("core/server/skills")
local TerrainSync = require("core/client/terrain-sync")
local UnlockManager = require("core/unlock/unlock-manager")

--- TODO:doc
-- @type Client
Client = Class("Client")

-- FIXME
Client.init_hooks = Hooks()
Client.play_hooks = Hooks()
Client.player_hooks = Hooks()
Client.reset_hooks = Hooks()
Client.speech_hooks = Hooks()
Client.start_hooks = Hooks()
Client.update_hooks = Hooks()

-- FIXME
Client.operators = {}
Operators = Client.operators
File:require_directory("core/client/operators")

Client.init = function(self)
	-- Initialize input.
	self.player_state = PlayerState()
	-- Initialize options.
	self.options = Options()
	-- Initialize graphics.
	Program:load_graphics()
	Reload:set_enabled(true)
	self.lighting = Lighting()
	-- Call the initialization hooks.
	self.init_hooks:call()
	-- Initialize the editor.
	--self.editor = Editor()
	-- Initialize data.
	self:reset_data()
	self.terrain_sync = TerrainSync()
	-- Initialize helper threads.
	self.threads = {}
	-- Execute the startup command.
	self.options:apply()
	self.start_hooks:call()
end

Client.deinit = function(self)
	self:terminate_game()
end

Client.add_speech_text = function(self, args)
	-- Add to the chat log.
	self:append_log("<" .. args.name .. "> " .. args.text)
	-- Call the speech hooks.
	self.speech_hooks:call(args)
end

--- Appends a message to the log.
-- @param self Client class.
-- @param text Text.
Client.append_log = function(self, text)
	local hud = Ui:get_hud("chat")
	if not hud then return end
	if not hud.widget then return end
	hud.widget:append(text)
end

--- Creates the static terrain and objects of the world.
-- @param self Client.
Client.create_world = function(self)
	if not Server.initialized then
		Main.objects:detach_all()
		Main.game.sectors:unload_all()
	end
end

Client.reset_data = function(self)
	-- Call the reset hooks.
	self.reset_hooks:call(secs)

	Operators.inventory:reset()
	Operators.quests:reset()
	--self.editor:reset()
	self.data = {}
	self.data.admin = {}
	self.data.book = {}
	self.data.connection = {}
	self.data.help = {page = "index"}
	self.data.inventory = {}
	self.data.load = {}
	self.data.map = {scale = 1}
	self.data.modifiers = {}
	self.data.skills = Skills()
	self.data.trading = {buy = {}, sell = {}, shop = {}}
	self.data.unlocks = UnlockManager()
	for k,v in pairs(Skillspec.dict_name) do
		local found = false
		for k1,v1 in pairs(v.requires) do found = true end
		self.data.skills[k] = {active = not found, value = false}
	end
end

--- Registers an initialization hook.
-- @param priority Priority.
-- @param hook Function.
Client.register_init_hook = function(self, priority, hook)
	self.init_hooks:register(priority, hook)
end

--- Registers a playing start hook.
-- @param priority Priority.
-- @param hook Function.
Client.register_play_hook = function(self, priority, hook)
	self.play_hooks:register(priority, hook)
end

--- Registers a player object change hook.
-- @param priority Priority.
-- @param hook Function.
Client.register_player_hook = function(self, priority, hook)
	self.player_hooks:register(priority, hook)
end

--- Registers a game reset hook.
-- @param priority Priority.
-- @param hook Function.
Client.register_reset_hook = function(self, priority, hook)
	self.reset_hooks:register(priority, hook)
end

--- Registers a speech hook.
-- @param priority Priority.
-- @param hook Function.
Client.register_speech_hook = function(self, priority, hook)
	self.speech_hooks:register(priority, hook)
end

--- Registers a start hook.
-- @param priority Priority.
-- @param hook Function.
Client.register_start_hook = function(self, priority, hook)
	self.start_hooks:register(priority, hook)
end

--- Registers an update hook.
-- @param priority Priority.
-- @param hook Function.
Client.register_update_hook = function(self, priority, hook)
	self.update_hooks:register(priority, hook)
end

Client.show_main_menu = function(self, settings)
	if settings and settings.join then
		Main.join:join_game()
	elseif settings and settings.host then
		Main.host:host_game()
	else
		Ui:set_state("mainmenu")
	end
end

Client.update = function(self, secs)
	self.update_hooks:call(secs)
end

--- Sets or unsets the text of the action label.
-- @param self Client class.
-- @param text String or nil.
Client.set_action_text = function(self, text)
	self.action_text = text
end

--- Updates the rendering style to match the current settings.
-- @param self Client.
Client.update_rendering_style = function(self)
end

--- Sets or unsets the active target.
-- @param self Client class.
-- @param text String or nil.
Client.set_target_text = function(self, text)
	self.action_text = text
end

Client.set_player_dead = function(self, value)
	if self.player_object then
		self.player_object.dead = value
	end
end

--- Starts a single player game.
-- @param self Client.
Client.start_single_player = function(self)
	-- Start the server.
	Main.game = Game("single", Main.settings.file)
	if not Main.game:start() then
		print("ERROR: Unsupported save file.")
		Main.game = nil
		return
	end
	Main.messaging:set_transmit_mode(true, true, nil)
	Server:init(false, true)
	if Main.settings.generate then
		Main.game:save(true)
	else
		Main.game:load()
	end
	-- Set information for the UI.
	Client.data.connection.mode = "single"
	Client.data.connection.text = "Starting the single player mode..."
	Client.data.connection.active = true
	Client.data.connection.connecting = false
	Client.data.connection.waiting = false
	-- Enter the start game mode.
	Client.data.load.next_state = "start-game"
	Ui:set_state("load")
end

--- Terminates the connection to the server.
-- @param self Client class.
Client.terminate_game = function(self)
	self.data.connection.active = false
	self.data.connection.connecting = false
	self.data.connection.waiting = false
	Main:end_game()
	self.terrain_sync:clear()
	self.player_object = nil
end

Client.get_connected = function(self)
	return self.data.connection.waiting
end

Client.get_player_object = function(self)
	return self.player_object
end

Client.set_player_object = function(self, v)
	self.player_object = v
	self.player_hooks:call(v)
end

return Client

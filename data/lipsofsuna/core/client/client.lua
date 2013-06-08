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
local Binding = require("core/client/binding")
local File = require("system/file")
local Hooks = require("system/hooks")
local Input = require("core/client/input")
local Lighting = require("core/client/lighting")
local Options = require("core/client/options")
local Physics = require("system/physics")
local PlayerState = require("core/client/player-state")
local Reload = require("system/reload")
local Simulation = require("core/client/simulation")
local Skills = require("core/server/skills")
local TerrainSync = require("core/client/terrain-sync")
local UnlockManager = require("core/server/unlock-manager")

--- TODO:doc
-- @type Client
Client = Class("Client")

-- FIXME
Client.init_hooks = Hooks()
Client.player_hooks = Hooks()
Client.reset_hooks = Hooks()
Client.speech_hooks = Hooks()
Client.update_hooks = Hooks()

-- FIXME
Client.operators = {}
Operators = Client.operators
File:require_directory("core/client/operators")

Client.init = function(self)
	-- Initialize input.
	self.input = Input()
	self.bindings = Binding
	self.player_state = PlayerState()
	-- Initialize options.
	self.options = Options()
	Operators.controls:init()
	Operators.controls:load()
	-- Initialize graphics.
	Program:load_graphics()
	Reload:set_enabled(true)
	self.lighting = Lighting()
	-- Call the initialization hooks.
	self.init_hooks:call(secs)
	-- Initialize the editor.
	--self.editor = Editor()
	-- Initialize data.
	self:reset_data()
	self.terrain_sync = TerrainSync()
	-- Initialize helper threads.
	self.threads = {}
	-- Execute the startup command.
	self.options:apply()
	if Settings.join then
		self:join_game()
	elseif Settings.host then
		self:host_game()
	elseif Settings.editor then
		Ui:set_state("editor")
	elseif Settings.benchmark then
		Ui:set_state("benchmark")
		self.benchmark = Benchmark()
	else
		Ui:set_state("mainmenu")
	end
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
		Game.objects:detach_all()
		Game.sectors:unload_all()
	end
end

Client.reset_data = function(self)
	-- Call the reset hooks.
	self.reset_hooks:call(secs)

	Operators.inventory:reset()
	Operators.spells:reset()
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

--- Registers an update hook.
-- @param priority Priority.
-- @param hook Function.
Client.register_update_hook = function(self, priority, hook)
	self.update_hooks:register(priority, hook)
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

--- Terminates the connection to the server.
-- @param self Client class.
Client.terminate_game = function(self)
	self.data.connection.active = false
	self.data.connection.connecting = false
	self.data.connection.waiting = false
	Game:deinit()
	self.terrain_sync:clear()
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

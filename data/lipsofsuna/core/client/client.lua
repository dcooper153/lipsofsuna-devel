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
local Database = require("system/database")
local EffectManager = require("core/client/effect-manager")
local File = require("system/file")
local FirstPersonCamera = require("core/client/first-person-camera")
local Input = require("core/client/input")
local Lighting = require("core/client/lighting")
local Network = require("system/network")
local Options = require("core/client/options")
local Physics = require("system/physics")
local PlayerState = require("core/client/player-state")
local Quickslots = require("core/quickslots/quickslots")
local Reload = require("system/reload")
local Simulation = require("core/client/simulation")
local Skills = require("core/server/skills")
local Sound = require("system/sound")
local TerrainSync = require("core/client/terrain-sync")
local ThirdPersonCamera = require("core/client/third-person-camera")
local UnlockManager = require("core/server/unlock-manager")

--- TODO:doc
-- @type Client
Client = Class("Client")

-- FIXME
Client.update_hooks = {}

-- FIXME
Client.operators = {}
Operators = Client.operators
File:require_directory("core/client/operators")

Client.init = function(self)
	self.effects = EffectManager()
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
	-- Initialize the UI.
	Ui:init()
	-- Initialize the database.
	self.db = Database("client.sqlite")
	self.db:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT PRIMARY KEY,value TEXT);")
	Quickslots:init()
	-- Initialize the editor.
	--self.editor = Editor()
	-- Initialize the camera.
	-- These need to be initialized before options since they'll be
	-- reconfigured when the options are loaded.
	self.camera1 = FirstPersonCamera()
	self.camera1:set_collision_mask(Game.PHYSICS_MASK_CAMERA)
	self.camera1:set_far(self.options.view_distance)
	self.camera1:set_fov(1.1)
	self.camera1:set_near(0.1)
	self.camera3 = ThirdPersonCamera()
	self.camera3:set_collision_mask(Game.PHYSICS_MASK_CAMERA)
	self.camera3:set_far(self.options.view_distance)
	self.camera3:set_fov(1.1)
	self.camera3:set_near(0.1)
	self:set_camera_mode("third-person")
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
	-- Play the sound effect.
	self.effects:play_object("chat1", args.object)
	-- Create a text bubble.
	local bounds = args.object:get_bounding_box()
	local offset = bounds.point.y + bounds.size.y + 0.5
	self.effects:create_speech_bubble{
		life = 5,
		fade = 1,
		object = args.object,
		offset = Vector(0,offset,0),
		text = args.text,
		text_color = {1,1,1,1},
		text_font = "medium"}
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
	Operators.camera:reset()
	Operators.chargen:reset()
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

--- Registers an update hook.
-- @param priority Priority.
-- @param hook Function.
Client.register_update_hook = function(self, priority, hook)
	local h = {priority = priority, hook = hook}
	for k,v in ipairs(self.update_hooks) do
		if priority < v.priority then
			table.insert(self.update_hooks, k, h)
			return
		end
	end
	table.insert(self.update_hooks, h)
end

Client.update = function(self, secs)
	for k,v in ipairs(self.update_hooks) do
		v.hook(secs)
	end
end

Client.update_camera = function(self)
	--Program:set_multisamples(Client.options.multisamples)
	Program:set_camera_far(self.camera:get_far())
	Program:set_camera_near(self.camera:get_near())
	Program:set_camera_position(self.camera:get_position())
	Program:set_camera_rotation(self.camera:get_rotation())
	local mode = Program:get_video_mode()
	local viewport = {0, 0, mode[1], mode[2]}
	self.camera:set_viewport(viewport)
	self.camera1:set_viewport(viewport)
	self.camera3:set_viewport(viewport)
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

Client.get_camera_mode = function(self)
	if self.camera == self.camera1 then
		return "first-person"
	else
		return "third-person"
	end
end

Client.get_connected = function(self)
	return self.data.connection.waiting
end

Client.get_player_object = function(self)
	return self.player_object
end

Client.set_camera_mode = function(self, v)
	if v == "first-person" then
		self.camera = self.camera1
	else
		self.camera = self.camera3
	end
	self.camera:reset()
end

Client.set_mouse_smoothing = function(self, v)
	local s = v and 0.7 or 1
	self.camera3:set_rotation_smoothing(s)
	self.camera3:set_position_smoothing(s)
end

Client.set_player_object = function(self, v)
	self.player_object = v
	self.camera = self.camera3
end

return Client

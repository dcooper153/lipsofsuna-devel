require "client/firstpersoncamera"
require "client/thirdpersoncamera"
require "client/model-builder"
require "client/render-object"
require "client/terrain-sync"
require "editor/editor"
require "common/skills"
require "common/unlocks"

Client = Class()
Client.class_name = "Client"

Operators = {}
File:require_directory("client/operators")

Client.init = function(self)
	-- Initialize the UI.
	Program:load_graphics()
	Theme:init()
	Ui:init()
	-- Initialize the database.
	Client.db = Database{name = "client.sqlite"}
	Client.db:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT PRIMARY KEY,value TEXT);")
	Quickslots:init()
	-- Initialize options.
	self.options = Options.inst
	Operators.controls:init()
	Operators.controls:load()
	-- Initialize the editor.
	self.editor = Editor()
	-- Initialize the camera.
	-- These need to be initialized before options since they'll be
	-- reconfigured when the options are loaded.
	self.camera1 = FirstPersonCamera{collision_mask = Physics.MASK_CAMERA, far = Options.inst.view_distance, fov = 1.1, near = 0.01}
	self.camera3 = ThirdPersonCamera{collision_mask = Physics.MASK_CAMERA, far = Options.inst.view_distance, fov = 1.1, near = 0.01}
	self.camera_mode = "third-person"
	-- Initialize data.
	self:reset_data()
	self.terrain_sync = TerrainSync()
	-- Initialize helper threads.
	self.threads = {}
end

Client.add_speech_text = function(self, args)
	-- Add to the chat log.
	Client:append_log("<" .. args.name .. "> " .. args.text)
	-- Play the sound effect.
	Effect:play_object("chat1", args.object)
	-- Create a text bubble.
	local bounds = args.object:get_bounding_box()
	local offset = bounds.point.y + bounds.size.y + 0.5
	TextBubble{
		life = 5,
		fade = 1,
		object = args.object,
		offset = Vector(0,offset,0),
		text = args.text,
		text_color = {1,1,1,1},
		text_font = "medium"}
end

Client.add_damage_text = function(self, args)
	TextBubble{
		life = 3,
		fade = 1,
		object = args.object,
		offset = Vector(0,2,0),
		text = args.text,
		text_color = args.color,
		text_font = "medium",
		velocity = Vector(0,-30)}
end

--- Appends a message to the log.
-- @param self Client class.
Client.append_log = function(self, text)
	local hud = Ui:get_hud("chat")
	if not hud then return end
	if not hud.widget then return end
	hud.widget:append(text)
end

--- Applies a world space quake.
-- @param self Client class.
-- @param point Quake point in world space.
-- @param magnitude Quake magnitude.
Client.apply_quake = function(self, point, magnitude)
	if point and magnitude and self.player_object then
		local dist = (self.player_object.position - point).length
		local quake = math.min(math.max(magnitude / (0.05 * dist * dist + 0.5), 0), 1)
		self.camera1.quake = math.max(self.camera1.quake or 0, quake)
		self.camera3.quake = math.max(self.camera3.quake or 0, quake)
	end
end

--- Creates the static terrain and objects of the world.
-- @param self Client.
Client.create_world = function(self)
	if not Server.initialized then
		for k,v in pairs(Object.objects) do v:detach() end
		Game.sectors:unload_world()
	end
end

Client.reset_data = function(self)
	Operators.camera:reset()
	Operators.chargen:reset()
	Operators.inventory:reset()
	Operators.spells:reset()
	Operators.quests:reset()
	self.editor:reset()
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
	self.data.unlocks = Unlocks
	self.data.unlocks:init()
	for k,v in pairs(Skillspec.dict_name) do
		local found = false
		for k1,v1 in pairs(v.requires) do found = true end
		self.data.skills[k] = {active = not found, value = false}
	end
end

Client.update = function(self, secs)
	-- Emit key repeat events.
	local t = Program.time
	for k,v in pairs(Binding.dict_press) do
		if t - v.time > 0.05 then
			v.type = "keyrepeat"
			v.mods = Binding.mods
			v.time = t
			Ui:handle_event(v)
		end
	end
	-- Update the user interface state.
	Ui:update(secs)
	-- Update the window size.
	if Ui.was_resized then
		local v = Program.video_mode
		self.options.window_width = v[1]
		self.options.window_height = v[2]
		self.options.fullscreen = v[3]
		self.options.vsync = v[4]
		self.options:save()
	end
	-- Update the simulation.
	Simulation:update(secs)
	-- Update the benchmark.
	if self.benchmark then
		self.benchmark:update(secs)
		return
	end
	-- Update the connection status.
	if self.connected and not Network.connected then
		self:terminate_game()
		self.options.host_restart = false
		self.data.connection.active = false
		self.data.connection.waiting = false
		self.data.connection.connecting = false
		self.data.connection.text = "Lost connection to the server!"
		self.data.load.next_state = "start-game"
		Ui.state = "load"
	end
	-- Update the player state.
	if self.player_object then
		PlayerState:update_pose(secs)
		PlayerState:update_rotation(secs)
		self.camera1.object = self.player_object
		self.camera3.object = self.player_object
		self.camera1:update(secs)
		self.camera3:update(secs)
		Lighting:update(secs)
		-- Sound playback.
		local p,r = self.player_object:find_node{name = "#neck", space = "world"}
		if p then
			Sound.listener_position = p
			Sound.listener_rotation = r
		else
			Sound.listener_position = self.player_object.position + Vector(0,1.5,0)
			Sound.listener_rotation = self.player_object.rotation
		end
		local vel = self.player_object.velocity
		if vel then Sound.listener_velocity = vel end
		-- Refresh the active portion of the map.
		self.player_object:refresh()
	end
	-- Update effects.
	-- Must be done after objects to ensure correct anchoring.
	if Game.initialized then
		for k in pairs(Game.scene_nodes_by_ref) do
			k:update(secs)
		end
	end
	-- Update text bubbles.
	-- Must be done after camera for the bubbles to stay fixed.
	for k in pairs(TextBubble.dict) do
		k:update(secs)
	end
	-- Update the 3D cursor.
	-- This really needs to be done every frame since the 3rd person
	-- camera suffers greatly from any big cursor position changes.
	if self.player_object and Ui.pointer_grab then
		PlayerState:pick_look()
	else
		Target.target_object = nil
	end
	-- FIXME
	if self.player_object then
		self:update_camera()
		local player_y = self.player_object.position.y
		local overworld_y = Map.heightmap.position.y - 10
		local overworld = (player_y > overworld_y)
		Map.heightmap.visible = overworld
		Lighting:set_dungeon_mode(not overworld)
		local wd = overworld and Options.inst.view_distance or Options.inst.view_distance_underground
		self.camera1.far = wd
		self.camera3.far = wd
	end
end

Client.update_camera = function(self)
	Program.hdr = Client.options.bloom_enabled
	Program.multisamples = Client.options.multisamples
	Program.camera_far = self.camera.far
	Program.camera_near = self.camera.near
	Program.camera_position = self.camera.position
	Program.camera_rotation = self.camera.rotation
	local mode = Program.video_mode
	local viewport = {0, 0, mode[1], mode[2]}
	self.camera.viewport = viewport
	self.camera1.viewport = viewport
	self.camera3.viewport = viewport
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
-- @param clss Client class.
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

Client:add_class_getters{
	camera_mode = function(self)
		if self.camera == self.camera1 then
			return "first-person"
		else
			return "third-person"
		end
	end,
	connected = function(self)
		return self.data.connection.waiting
	end,
	player_object = function(self)
		return rawget(self, "__player_object")
	end}

Client:add_class_setters{
	camera_mode = function(self, v)
		if v == "first-person" then
			self.camera = self.camera1
		else
			self.camera = self.camera3
		end
		self.camera:reset()
	end,
	mouse_smoothing = function(self, v)
		local s = v and 0.7 or 1
		if self.player_object then
			self.player_object.rotation_smoothing = s
		end
		self.camera3.rotation_smoothing = s
		self.camera3.position_smoothing = s
	end,
	notification_text = function(self, v)
		local hud = Ui:get_hud("notification")
		if not hud then return end
		hud.widget.text = v
	end,
	player_object = function(self, v)
		rawset(self, "__player_object", v)
		Camera.mode = "third-person"
	end}

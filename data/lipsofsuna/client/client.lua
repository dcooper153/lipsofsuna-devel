require "client/firstpersoncamera"
require "client/thirdpersoncamera"
require "editor/editor"
require "common/skills"
require "common/unlocks"

Client = Class()
Client.class_name = "Client"

Operators = {}

for k,v in pairs(File:scan_directory("client/operators/")) do
	require("client/operators/" .. string.gsub(v, "([^.]*).*", "%1"))
end

-- Initialize the database.
-- FIXME: Should be initialized in the initializer.
Client.db = Database{name = "client.sqlite"}
Client.db:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT PRIMARY KEY,value TEXT);")

Client.init = function(self)
	-- Initialize options.
	self.options = Options.inst
	self.controls = ConfigFile{name = "controls.cfg"}
	self:load_controls()
	self:update_rendering_style()
	-- Initialize the world.
	self.sectors = Sectors{database = Client.db, save_objects = false}
	self.sectors:erase_world()
	-- Initielize the editor.
	self.editor = Editor()
	-- Initialize the camera.
	-- These need to be initialized before options since they'll be
	-- reconfigured when the options are loaded.
	self.camera1 = FirstPersonCamera{collision_mask = Physics.MASK_CAMERA, far = Options.inst.view_distance, fov = 1.1, near = 0.01}
	self.camera3 = ThirdPersonCamera{collision_mask = Physics.MASK_CAMERA, far = Options.inst.view_distance, fov = 1.1, near = 0.01}
	self.camera_mode = "third-person"
	-- Initialize data.
	self:reset_data()
	-- Initialize helper threads.
	self.threads = {}
	self.threads.model_builder = Thread("client/threads/modelbuilder.lua")
end

Client.add_speech_text = function(self, args)
	-- FIXME
	Sound:effect{object = args.object, effect = "spring-000"}
	-- Add to the chat log.
	Client:append_log("<" .. args.name .. "> " .. args.text)
	-- Create a text bubble.
	local bounds = args.object.bounding_box
	local offset = bounds.point.y + bounds.size.y + 0.5
	TextBubble{
		life = 5,
		fade = 1,
		object = args.object,
		position = Vector(0,offset,0),
		text = args.text,
		text_color = {1,1,1,1},
		text_font = "medium"}
end

Client.add_damage_text = function(self, args)
	TextBubble{
		life = 3,
		fade = 1,
		object = args.object,
		position = Vector(0,2,0),
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
	for k,v in pairs(Object.objects) do v:detach() end
	self.sectors:erase_world()
end

Client.reset_data = function(self)
	Operators.camera:reset()
	Operators.chargen:reset()
	Operators.inventory:reset()
	Operators.spells:reset()
	Operators.quests:reset()
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

Client.update = function(self)
	-- FIXME
	if not self.player_object then return end
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
	-- Outline rendering.
	if self.options.outlines_enabled then
		Render.material_scheme = "Default"
	else
		Render.material_scheme = "no-outline"
	end
	-- Bloom.
	Render:remove_compositor("bloom1")
	if self.options.bloom_enabled then
		Render:add_compositor("bloom1")
	end
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
	-- Disconnect from the server.
	Network:shutdown()
	self.data.connection.active = false
	self.data.connection.connecting = false
	self.data.connection.waiting = false
	-- Terminate the local server.
	if self.threads.server then
		self.threads.server.quit = true
		while not self.threads.server.done do
		end
		self.threads.server = nil
	end
	-- Clear the world.
	for k,v in pairs(Object.objects) do v:detach() end
	Client.player_object = nil
	self.sectors:erase_world()
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
	player_object = function(self) return Player.object end}

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
		local s = v and 0.15 or 1
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
		Player.object = v
		Camera.mode = "third-person"
	end}

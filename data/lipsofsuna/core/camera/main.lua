local Camera = require("core/camera/camera")
local Client = require("core/client/client")
local FirstPersonCamera = require("core/camera/first-person-camera")
local Program = require("system/core")
local Simulation = require("core/client/simulation")
local ThirdPersonCamera = require("core/camera/third-person-camera")

Client.camera_manager = Camera
Client.operators.camera = Camera

Client:register_init_hook(25, function()
	-- Initialize the camera.
	-- These need to be initialized before options since they'll be
	-- reconfigured when the options are loaded.
	Client.camera1 = FirstPersonCamera()
	Client.camera1:set_collision_mask(Game.PHYSICS_MASK_CAMERA)
	Client.camera1:set_far(Client.options.view_distance)
	Client.camera1:set_fov(1.1)
	Client.camera1:set_near(0.1)
	Client.camera3 = ThirdPersonCamera()
	Client.camera3:set_collision_mask(Game.PHYSICS_MASK_CAMERA)
	Client.camera3:set_far(Client.options.view_distance)
	Client.camera3:set_fov(1.1)
	Client.camera3:set_near(0.1)
	Client:set_camera_mode("third-person")
end)

Client:register_player_hook(25, function()
	Client:set_mouse_smoothing(Client.options.mouse_smoothing)
	Client.camera = Client.camera3
end)

Client:register_reset_hook(25, function()
	Client.camera_manager:reset()
end)

Client:register_update_hook(25, function(secs)
	-- Update the camera.
	if Client.player_object then
		Client.camera1.object = Client.player_object
		Client.camera3.object = Client.player_object
		Client.camera1:update(secs)
		Client.camera3:update(secs)
		Client.lighting:update(secs)
	end
	-- Update targeting.
	if Client.player_object then
		local r1,r2 = Client.camera1:get_picking_ray()
		if r1 then
			local p,o = Simulation:pick_scene_by_ray(r1, r2)
			Client.player_state:set_targeted_object(o)
			Client.player_state.crosshair = (p or r2) - (r2 - r1):normalize() * 0.1
		end
	else
		Client.player_state:set_targeted_object()
		Client.player_state.crosshair = nil
	end
	-- Update the viewport.
	if Client.player_object then
		--Program:set_multisamples(Client.options.multisamples)
		Program:set_camera_far(Client.camera:get_far())
		Program:set_camera_near(Client.camera:get_near())
		Program:set_camera_position(Client.camera:get_position())
		Program:set_camera_rotation(Client.camera:get_rotation())
		local mode = Program:get_video_mode()
		local viewport = {0, 0, mode[1], mode[2]}
		Client.camera:set_viewport(viewport)
		Client.camera1:set_viewport(viewport)
		Client.camera3:set_viewport(viewport)
		Client.lighting:set_dungeon_mode(false)
		Client.camera1:set_far(Client.options.view_distance)
		Client.camera3:set_far(Client.options.view_distance)
	end
end)

-- FIXME
Client.get_camera_mode = function(self)
	if self.camera == self.camera1 then
		return "first-person"
	else
		return "third-person"
	end
end

-- FIXME
Client.set_camera_mode = function(self, v)
	if v == "first-person" then
		self.camera = self.camera1
	else
		self.camera = self.camera3
	end
	self.camera:reset()
end

-- FIXME
Client.set_mouse_smoothing = function(self, v)
	local s = v and 0.7 or 1
	self.camera3:set_rotation_smoothing(s)
	self.camera3:set_position_smoothing(s)
end

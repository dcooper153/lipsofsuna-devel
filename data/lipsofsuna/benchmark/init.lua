--- Provides a simple graphical benchmark.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module benchmark.init
-- @alias Benchmark

if Settings.server then return end

local Camera = require("system/camera")
local Class = require("system/class")
local Light = require("system/light")
local Quaternion = require("system/math/quaternion")
local RenderModel = require("system/render-model")
local RenderObject = require("system/render-object")
local Simulation = require("core/client/simulation")
local TerrainManager = require("core/server/terrain-manager")
local Vector = require("system/math/vector")

--- Provides a simple graphical benchmark.
-- @type Benchmark
Benchmark = Class("Benchmark")

--- Creates the benchmark.
-- @param clss Benchmark class.
-- @return Benchmark.
Benchmark.new = function(clss)
	local self = Class.new(clss)
	-- Initialize the game.
	Game:init("benchmark")
	Game.sectors.unload_time = nil
	-- Create the objects.
	self.objects = setmetatable({}, {__mode = "kv"})
	local anims = {"idle", "walk", "strafe left"}
	local spec = Actorspec:find{name = "Lips guard archer"}
	for i = 1,20 do
		local o = Simulation:create_object_by_spec(spec)
		local a = i/20*2*math.pi
		o:set_position(Vector(500,500,500) + Vector(math.cos(a), 0, math.sin(a)):multiply(i/4), true)
		o:set_visible(true)
		o.render:init(o)
		o.render:add_animation(anims[i % 3 + 1])
		self.objects[o:get_id()] = o
		self.object = o
	end
	-- Create a static object.
	local spec = Staticspec:find{name = "statictree1"}
	local o = Simulation:create_object_by_spec(spec)
	o:set_position(Vector(500,500,497))
	o:set_visible(true)
	o.render:init(o)
	-- Create the camera.
	self.translation = Vector(-10, 30, -30)
	self.camera = Camera()
	self.camera:set_far(1000)
	self.camera:set_near(0.3)
	self.camera:set_mode("first-person")
	-- Create the light.
	self.light = Light()
	self.light:set_ambient{0.3,0.3,0.3,1.0}
	self.light:set_diffuse{0.6,0.6,0.6,1.0}
	self.light:set_equation{1.0,0.0,0.01}
	self.light:set_enabled(true)
	-- Create the terrain.
	self.terrain_timer1 = 0
	self.terrain_timer2 = 1
	self.terrain = TerrainManager(16, 1, nil, false, true, true)
	self.terrain:refresh_point(Vector(500, 0, 500), 64)
	return self
end

--- Removes benchmark objects from the scene.
-- @param self Benchmark.
Benchmark.close = function(self)
	-- Restore the normal map state.
	Game.objects:detach_all()
	Game.sectors:unload_all()
	self.terrain:unload_all()
	self.light:set_enabled(false)
	Client.sectors.unload_time = 10
end

--- Updates the benchmark scene.
-- @param self Benchmark.
-- @param secs Seconds since the last update.
Benchmark.update = function(self, secs)
	-- Update the camera.
	local camctr = Vector(500,500,500)
	local campos = camctr + self.translation
	local camrot = Quaternion{dir = camctr - campos, up = Vector(0,1,0)}
	self.camera:set_target_position(campos)
	self.camera:set_target_rotation(camrot)
	self.camera:update(secs)
	self.camera:warp()
	Client.camera = self.camera
	Client:update_camera()
	-- Update lighting.
	Client.lighting:update(secs)
	-- Modify objects.
	for k,v in pairs(self.objects) do
		if not v.model_rebuild_timer then
			v.model_rebuild_timer = 2
			v.body_style = {}
			for i = 1,9 do
				v.body_style[i] = math.random()
			end
		end
	end
	-- Modify terrain.
	-- TODO
	self.terrain:update(secs)
	-- Update the terrain timer.
	self.terrain_timer1 = self.terrain_timer1 + 1
	if self.terrain_timer1 > 400 then
		self.terrain_timer1 = 0
		if self.terrain_timer2 == 1 then
			self.terrain_timer2 = 2
		else
			self.terrain_timer2 = 1
		end
	end
end

return Benchmark

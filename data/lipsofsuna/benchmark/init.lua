local Simulation = require("core/client/simulation")

Benchmark = Class()
Benchmark.class_name = "Benchmark"

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
		o.render:init(o)
		o.render:add_animation(anims[i % 3 + 1])
		o:set_visible(true)
		self.objects[o.id] = o
		self.object = o
	end
	-- Setup terrain benchmarking.
	self.terrain_offset = Vector(500,500,500):multiply(Voxel.tile_scale):floor():subtract_xyz(10,1,7)
	self.terrain_timer1 = 0
	self.terrain_timer2 = 1
	-- Create the camera.
	self.translation = Vector(-5, 2, -15)
	self.camera = Camera{far = 1000.0, near = 0.3, mode = "first-person"}
	-- Create the light.
	self.light = Light{ambient = {0.3,0.3,0.3,1.0}, diffuse={0.6,0.6,0.6,1.0}, equation={1.0,0.0,0.01}}
	self.light.enabled = true
	return self
end

Benchmark.close = function(self)
	-- Restore the normal map state.
	for k,v in pairs(Object.objects) do v:detach() end
	Client.sectors:unload_world()
	self.light.enabled = false
	Client.sectors.unload_time = 10
end

--- Updates the benchmark scene.
-- @param self Benchmark.
-- @param secs Seconds since the last update.
Benchmark.update = function(self, secs)
	-- Update the camera.
	self.camera.target_position = self.object.position + self.translation
	self.camera.target_rotation = Quaternion{axis = Vector(0, 1, 0), angle = math.pi}
	self.camera:update(secs)
	self.camera:warp()
	Client.camera = self.camera
	Client:update_camera()
	-- Update lighting.
	Client.lighting:update(secs)
	-- Modify objects.
	for k,v in pairs(self.objects) do
		v:activate(5)
		if not v.model_rebuild_timer then
			v.model_rebuild_timer = 2
			v.body_style = {}
			for i = 1,9 do
				v.body_style[i] = math.random()
			end
		end
	end
	-- Modify terrain.
	local x = self.terrain_timer1 % 20
	local z = math.floor(self.terrain_timer1 / 20)
	local q = self.terrain_offset:copy():add_xyz(x, 0, z)
	Voxel:set_tile(q, self.terrain_timer2)
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

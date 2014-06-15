--- Camera for the benchmark.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module benchmark.camera
-- @alias BenchmarkCamera

local Camera = require("system/camera")
local Class = require("system/class")
local Quaternion = require("system/math/quaternion")
local Vector = require("system/math/vector")

--- Camera for the benchmark.
-- @type BenchmarkCamera
local BenchmarkCamera = Class("BenchmarkCamera", Camera)

--- Creates a new benchmark camera.
-- @param clss BenchmarkCamera class.
-- @return BenchmarkCamera.
BenchmarkCamera.new = function(clss)
	local self = Camera.new(clss)
	self:set_far(1000)
	self:set_near(0.3)
	self:set_mode("first-person")
	return self
end

--- Updates the camera transformation.
-- @param self Camera.
-- @param secs Seconds since the last update.
BenchmarkCamera.update = function(self, secs)
	if Main.benchmark then
		Client.camera_manager:set_camera_mode("benchmark")
		local camctr = Vector(505,500,500)
		local campos = camctr + Main.benchmark.translation
		local dir = camctr - campos
		local camrot = Quaternion:new_from_dir(dir.x,dir.y,dir.z, 0,1,0)
		self:set_target_position(campos)
		self:set_target_rotation(camrot)
		Camera.update(self, secs)
		self:warp()
	end
end

return BenchmarkCamera

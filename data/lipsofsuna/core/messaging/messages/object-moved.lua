-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local Quaternion = require("system/math/quaternion")

Main.messaging:register_message{
	name = "object moved",
	server_to_client_encode = function(self, id, position, rotation, tilt, velocity)
		return {"uint32", id,
			"float", position.x, "float", position.y, "float", position.z,
			"float", rotation.x, "float", rotation.y, "float", rotation.z, "float", rotation.w,
			"float", tilt,
			"float", velocity.x, "float", velocity.y, "float", velocity.z}, true
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,x,y,z,rx,ry,rz,rw,t,vx,vy,vz = packet:read("uint32",
			"float", "float", "float",
			"float", "float", "float", "float",
			"float",
			"float", "float", "float")
		if not ok then return end
		return {id, Vector(x,y,z), Quaternion(rx,ry,rz,rw), t, Vector(vx,vy,vz)}
	end,
	server_to_client_handle = function(self, id, position, rotation, tilt, velocity)
		-- Get the object.
		local o = Main.objects:find_by_id(id)
		if not o then return end
		-- Redundant in single player.
		if o:has_server_data() then
			o.render:set_position(position)
			return
		end
		-- Set the target interpolation position.
		o:set_position(position, true)
		o:set_rotation(rotation, true)
		o:set_tilt_angle(tilt, true)
		o:set_velocity(velocity, true)
		if o.prediction then
			o.prediction:mark()
		end
		-- Adjust time scaling of movement animations.
		if o.spec and o.spec.speed_walk then
			local a = o.render and o.render:get_animation{channel = 1}
			if a then
				local ref_speed
				local ref_scale
				local map = {
					["run"] = o.spec.speed_run,
					["run left"] = o.spec.speed_run,
					["run right"] = o.spec.speed_run,
					["strafe left"] = o.spec.speed_run,
					["strafe right"] = o.spec.speed_run,
					["walk"] = o.spec.speed_walk,
					["walk back"] = o.spec.speed_run}
				for k,v in pairs(o.spec.animations) do
					if v.animation == a.animation and map[k] then
						ref_speed = map[k]
						ref_scale = v.time_scale or 1
						break
					end
				end
				if ref_speed then
					local speed = Vector(velocity.x, 0, velocity.z).length
					local scale = speed / math.max(0.1, ref_speed)
					a.time_scale = ref_scale * scale
					o:animate(a)
				end
			end
		end
	end}

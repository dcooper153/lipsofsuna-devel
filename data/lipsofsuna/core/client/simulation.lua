--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.simulation
-- @alias Simulation

local Actor = require("core/objects/actor")
local Class = require("system/class")
local Color = require("system/color")
local Item = require("core/objects/item")
local Obstacle = require("core/objects/obstacle")
local Physics = require("system/physics")
local PhysicsConsts = require("core/physics/consts")
local Spell = require("core/objects/spell")
local Staticobject = require("core/objects/static")

--- TODO:doc
-- @type Simulation
local Simulation = Class("Simulation")

--- Creates an object by a spec.
-- @param self Simulation.
-- @param spec Object spec.
-- @param id Unique ID, or nil.
-- @return Object.
Simulation.create_object_by_spec = function(self, spec, id)
	-- Create the object.
	local c
	if spec.type == "actor" then c = "Actor"
	elseif spec.type == "item" then c = "Item"
	elseif spec.type == "spell" then c = "Spell"
	elseif spec.type == "obstacle" then c = "Obstacle"
	elseif spec.type == "static" then c = "Static"
	else
		return
	end
	local o = Main.objects:create_object(c, id)
	o:set_spec(spec)
	-- Apply presets.
	if spec.preset then
		local preset = Main.specs:find_by_name("ActorPresetSpec", spec.preset)
		if preset then
			preset = preset:get_chargen()
			local copy_table = function(t)
				if not t then return end
				local u = {}
				for k,v in pairs(t) do u[k] = v end
				return u
			end
			local copy_color = function(t)
				local u = copy_table(t)
				if not u then return end
				return Color:float_to_ubyte(Color:hsv_to_rgb(u))
			end
			o.body_scale = preset.height
			o.body_sliders = copy_table(preset.body)
			o.brow_style = preset.brow_style
			o.eye_color = copy_color(preset.eye_color)
			o.eye_style = preset.eye_style
			o.face_sliders = copy_table(preset.face)
			o.face_style = preset.face_style
			o.hair_color = copy_color(preset.hair_color)
			o.hair_style = preset.hair_style
			o.head_style = preset.head_style
			o.mouth_style = preset.mouth_style
			o.skin_color = copy_color(preset.skin_color)
			o.skin_style = preset.skin_style
		end
	end
	-- Return the object.
	return o
end

--- Picks an object or a tile from the scene based on a ray.
-- @param clss Simulation class.
-- @param camera Camera whose picking ray to use.
-- @param ignore Table of objects to ignore, or nil for none.
-- @return Position vector, object, tile vector.
Simulation.pick_scene_by_camera = function(clss, camera, ignore)
	local r1,r2 = camera:get_picking_ray()
	return clss:pick_scene_by_ray(r1, r2, ignore)
end

--- Picks an object or a tile from the scene based on a ray.
-- @param clss Simulation class.
-- @param ray1 Start point of the ray.
-- @param ray2 End point of the ray.
-- @param ignore Table of objects to ignore, or nil for none.
-- @return Position vector, object, tile vector.
Simulation.pick_scene_by_ray = function(clss, ray1, ray2, ignore)
	local best_object = nil
	local best_point = nil
	local best_dist = nil
	-- Pick from the scene.
	local ret = Physics:cast_ray(ray1, ray2, PhysicsConsts.MASK_PICK, ignore)
	if not ret then return end
	return ret.point, ret.object and Main.objects:find_by_id(ret.object), ret.tile
end

return Simulation

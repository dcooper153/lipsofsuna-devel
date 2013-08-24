--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.render_utils
-- @alias RenderUtils

local Animation = require("system/animation")
local Class = require("system/class")
local LightEffect = require("core/effect/light-effect") --FIXME
local ParticleEffect = require("core/effect/particle-effect") --FIXME

--- TODO:doc
-- @type RenderUtils
local RenderUtils = Class("RenderUtils")

--- Creates a scale animation for an actor.
-- @param self Render utils.
-- @param spec Actor spec.
-- @param scale_body Scale ratio for the body. Nil for none.
-- @param scale_head Scale ratio for the body. Nil for none.
-- @return Animation arguments, or nil.
RenderUtils.create_scale_animation = function(self, spec, scale_body, scale_head)
	local anim = Animation("scale")
	if scale_body then
		local min = spec.body_scale_min
		local max = spec.body_scale_max
		if min or max then
			local r = scale_body
			local s = (min or 1) * (1 - r) + (max or 1) * r
			anim:set_transform{frame = 1, node = "hips_main", scale = s}
		end
	end
	if scale_head then
		local min = spec.head_scale_min
		local max = spec.head_scale_max
		if min or max then
			local r = scale_head
			local s = (min or 1) * (1 - r) + (max or 1) * r
			anim:set_transform{frame = 1, node = "head", scale = s}
		end
	end
	return {channel = Animation.CHANNEL_CUSTOMIZE, animation = anim, fade_in = 0,
		fade_out = 0, permanent = true, replace = true, weight = 0, weight_scale = 1000}
end

--- Creates special effects for an object spec.
-- @param self Render utils.
-- @param parent Parent object for the effects to track.
-- @param spec Object spec of any type.
-- @return List of special effect objects.
RenderUtils.create_special_effects = function(self, parent, spec)
	local result = {}
	local effects = spec.get_special_effects and spec:get_special_effects()
	if effects then
		for k,v in pairs(effects) do
			if v.light then
				local effect = LightEffect{
					ambient = v.light_ambient,
					diffuse = v.light_diffuse,
					equation = v.light_equation,
					parent = parent,
					parent_node = v.node,
					position_mode = "node"}
				table.insert(result, fx)
			end
			if v.particle then
				local effect = ParticleEffect{
					life = v.particle_life,
					parent = parent,
					parent_node = v.node,
					particle = v.particle,
					position_mode = "node"}
				table.insert(result, effect)
			end
		end
	end
	return result
end

return RenderUtils



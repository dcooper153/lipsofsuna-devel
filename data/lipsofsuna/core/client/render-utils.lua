require "system/class"

RenderUtils = Class()

--- Creates a scale animation for an actor.
-- @param self Render utils.
-- @param spec Actor spec.
-- @param scale Scale ratio.
-- @return Animation arguments, or nil.
RenderUtils.create_scale_animation = function(self, spec, scale)
	local min = spec.body_scale_min
	local max = spec.body_scale_max
	if not min and not max then return end
	local r = scale or 0.5
	local s = (min or 1) * (1 - r) + (max or 1) * r
	local anim = Animation("scale")
	anim:set_transform{frame = 1, node = "mover", scale = s}
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

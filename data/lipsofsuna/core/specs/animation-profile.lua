--- Animation profile specification for actors.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.animation_profile
-- @alias AnimationProfileSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- Animation profile specification for actors.
-- @type AnimationProfileSpec
local AnimationProfileSpec = Spec:register("AnimationProfileSpec", "animation profile", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "animations", type = "dict", dict = {type = "string"}, default = {}, description = "Dictionary of animation specs.", details = {values = {spec = "Animationspec"}}},
	{name = "inherit", type = "list", list = {type = "string", details = {value = {spec = "AnimationProfileSpec"}}}, default = {}, description = "List of inherited profiles."}
})

--- Creates a new animation profile spec.
-- @param clss AnimationProfileSpec class.
-- @param args Arguments.
-- @return Animation profile spec.
AnimationProfileSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets the animation spec for the given animation in the profile.
-- @param self Animation profile spec.
-- @param name Animation name in the profile.
-- @return Animation spec, or nil.
AnimationProfileSpec.get_animation = function(self, name)
	-- Try primary animations.
	local aname = self.animations[name]
	if aname then
		return Animationspec:find{name = aname}
	end
	-- Try inherited animations.
	for k,v in pairs(self.inherit) do
		local profile = self.dict_name[v]
		local anim = profile:get_animation(name)
		if anim then return anim end
	end
end

return AnimationProfileSpec

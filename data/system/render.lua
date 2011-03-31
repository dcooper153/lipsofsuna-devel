if not Program:load_extension("render") then
	error("loading extension `render' failed")
end

local light_getters = {
	ambient = function(s) return s:get_ambient() end,
	diffuse = function(s) return s:get_diffuse() end,
	enabled = function(s) return s:get_enabled() end,
	equation = function(s) return s:get_equation() end,
	position = function(s) return s:get_position() end,
	priority = function(s) return s:get_priority() end,
	rotation = function(s) return s:get_rotation() end,
	shadow_casting = function(s) return s:get_shadow_casting() end,
	shadow_far = function(s) return s:get_shadow_far() end,
	shadow_near = function(s) return s:get_shadow_near() end,
	specular = function(s) return s:get_specular() end,
	spot_cutoff = function(s) return s:get_spot_cutoff() end,
	spot_exponent = function(s) return s:get_spot_exponent() end}

local light_setters = {
	ambient = function(s, v) s:set_ambient(v) end,
	diffuse = function(s, v) s:set_diffuse(v) end,
	enabled = function(s, v) s:set_enabled(v) end,
	equation = function(s, v) s:set_equation(v) end,
	position = function(s, v) s:set_position(v) end,
	priority = function(s, v) s:set_priority(v) end,
	rotation = function(s, v) s:set_rotation(v) end,
	shadow_casting = function(s, v) s:set_shadow_casting(v) end,
	shadow_far = function(s, v) s:set_shadow_far(v) end,
	shadow_near = function(s, v) s:set_shadow_near(v) end,
	specular = function(s, v) s:set_specular(v) end,
	spot_cutoff = function(s, v) s:set_spot_cutoff(v) end,
	spot_exponent = function(s, v) s:set_spot_exponent(v) end}

Light.getter = function(self, key)
	local lightgetterfunc = light_getters[key]
	if lightgetterfunc then return lightgetterfunc(self) end
	return Class.getter(self, key)
end

Light.setter = function(self, key, value)
	local lightsetterfunc = light_setters[key]
	if lightsetterfunc then return lightsetterfunc(self, value) end
	return Class.setter(self, key, value)
end

if not Program:load_extension("tiles") then
	error("loading extension `tiles' failed")
end

local mat_getters = {
	cullface = function(s) return s:get_cullface() end,
	diffuse = function(s) return s:get_diffuse() end,
	flags = function(s) return s:get_flags() end,
	friction = function(s) return s:get_friction() end,
	id = function(s) return s:get_id() end,
	name = function(s) return s:get_name() end,
	shader = function(s) return s:get_shader() end,
	shininess = function(s) return s:get_shininess() end,
	specular = function(s) return s:get_specular() end,
	texture = function(s) return s:get_texture() end,
	texture_scale = function(s) return s:get_texture_scale() end,
	type = function(s) return s:get_type() end}

local mat_setters = {
	cullface = function(s, v) s:set_cullface(v) end,
	diffuse = function(s, v) s:set_diffuse(v) end,
	flags = function(s, v) s:set_flags(v) end,
	friction = function(s, v) s:set_friction(v) end,
	name = function(s, v) s:set_name(v) end,
	shader = function(s, v) s:set_shader(v) end,
	shininess = function(s, v) s:set_shininess(v) end,
	specular = function(s, v) s:set_specular(v) end,
	texture = function(s, v) s:set_texture(v) end,
	texture_scale = function(s, v) s:set_texture_scale(v) end,
	type = function(s, v) s:set_type(v) end}

Material.getter = function(self, key)
	local matgetterfunc = mat_getters[key]
	if matgetterfunc then return matgetterfunc(self) end
	return Class.getter(self, key)
end

Material.setter = function(self, key, value)
	local matsetterfunc = mat_setters[key]
	if matsetterfunc then return matsetterfunc(self, value) end
	return Class.setter(self, key, value)
end

if not Program:load_extension("camera") then
	error("loading extension `camera' failed")
end

local camera_getters = {
	collision_group = function(s) return s:get_collision_group() end,
	collision_mask = function(s) return s:get_collision_mask() end,
	fov = function(s) return s:get_fov() end,
	mode = function(s) return s:get_mode() end,
	modelview = function(s) return s:get_modelview() end,
	position = function(s) return s:get_position() end,
	position_smoothing = function(s) return s:get_position_smoothing() end,
	projection = function(s) return s:get_projection() end,
	rotation = function(s) return s:get_rotation() end,
	rotation_smoothing = function(s) return s:get_rotation_smoothing() end,
	target_position = function(s) return s:get_target_position() end,
	target_rotation = function(s) return s:get_target_rotation() end,
	viewport = function(s) return s:get_viewport() end}

local camera_setters = {
	collision_group = function(s, v) s:set_collision_group(v) end,
	collision_mask = function(s, v) s:set_collision_mask(v) end,
	far = function(s, v) s:set_far(v) end,
	fov = function(s, v) s:set_fov(v) end,
	mode = function(s, v) s:set_mode(v) end,
	near = function(s, v) s:set_near(v) end,
	position_smoothing = function(s, v) s:set_position_smoothing(v) end,
	rotation_smoothing = function(s, v) s:set_rotation_smoothing(v) end,
	target_position = function(s, v) s:set_target_position(v) end,
	target_rotation = function(s, v) s:set_target_rotation(v) end,
	viewport = function(s, v) s:set_viewport(v) end}

Camera.getter = function(self, key)
	local cameragetterfunc = camera_getters[key]
	if cameragetterfunc then return cameragetterfunc(self) end
	return Class.getter(self, key)
end

Camera.setter = function(self, key, value)
	local camerasetterfunc = camera_setters[key]
	if camerasetterfunc then return camerasetterfunc(self, value) end
	return Class.setter(self, key, value)
end

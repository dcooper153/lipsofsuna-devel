local quat_getters = {
	conjugate = function(s) return s:get_conjugate() end,
	euler = function(s) return s:get_euler() end,
	length = function(s) return s:get_length() end,
	w = function(s) return s:get_w() end,
	x = function(s) return s:get_x() end,
	y = function(s) return s:get_y() end,
	z = function(s) return s:get_z() end}

local quat_setters = {
	length = function(s, v) s:set_length(v) end,
	w = function(s, v) s:set_w(v) end,
	x = function(s, v) s:set_x(v) end,
	y = function(s, v) s:set_y(v) end,
	z = function(s, v) s:set_z(v) end}

Quaternion.getter = function(self, key)
	local quatgetterfunc = quat_getters[key]
	if quatgetterfunc then return quatgetterfunc(self) end
	return Class.getter(self, key)
end

Quaternion.setter = function(self, key, value)
	local quatsetterfunc = quat_setters[key]
	if quatsetterfunc then return quatsetterfunc(self, value) end
	return Class.setter(self, key, value)
end

local vector_getters = {
	length = function(s) return s:get_length() end,
	x = function(s) return s:get_x() end,
	y = function(s) return s:get_y() end,
	z = function(s) return s:get_z() end}

local vector_setters = {
	length = function(s, v) s:set_length(v) end,
	x = function(s, v) s:set_x(v) end,
	y = function(s, v) s:set_y(v) end,
	z = function(s, v) s:set_z(v) end}

Vector.getter = function(self, key)
	local vectorgetterfunc = vector_getters[key]
	if vectorgetterfunc then return vectorgetterfunc(self) end
	return Class.getter(self, key)
end

Vector.setter = function(self, key, value)
	local vectorsetterfunc = vector_setters[key]
	if vectorsetterfunc then return vectorsetterfunc(self, value) end
	return Class.setter(self, key, value)
end

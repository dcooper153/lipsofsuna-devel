local getters = {
	length = function(s) return s:get_length() end,
	x = function(s) return s:get_x() end,
	y = function(s) return s:get_y() end,
	z = function(s) return s:get_z() end}

local setters = {
	length = function(s, v) s:set_length(v) end,
	x = function(s, v) s:set_x(v) end,
	y = function(s, v) s:set_y(v) end,
	z = function(s, v) s:set_z(v) end}

Vector.getter = function(self, key)
	local vectorgetterfunc = getters[key]
	if vectorgetterfunc then return vectorgetterfunc(self) end
	return Class.getter(self, key)
end

Vector.setter = function(self, key, value)
	local vectorsetterfunc = setters[key]
	if vectorsetterfunc then return vectorsetterfunc(self, value) end
	return Class.setter(self, key, value)
end

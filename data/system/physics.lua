if not Program:load_extension("physics") then
	error("loading extension `physics' failed")
end

Physics.getter = function(self, key)
	if key == "enable_simulation" then return self:get_enable_simulation() end
	return Class.getter(self, key)
end

Physics.setter = function(self, key, value)
	if key == "enable_simulation" then return self:set_enable_simulation(value) end
	return Class.setter(self, key, value)
end

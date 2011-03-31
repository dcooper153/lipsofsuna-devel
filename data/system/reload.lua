if not Program:load_extension("reload") then
	error("loading extension `reload' failed")
end

Reload.getter = function(self, key)
	if key == "enabled" then return self:get_enabled() end
	return Class.getter(self, key)
end

Reload.setter = function(self, key, value)
	if key == "enabled" then return self:set_enabled(value) end
	return Class.setter(self, key, value)
end

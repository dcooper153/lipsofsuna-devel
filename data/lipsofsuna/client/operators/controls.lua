--- Loads controls from the configuration file.
-- @param self Client.
Client.load_controls = function(self)
	local translate = function(k)
		if not k or k == "none" then return end
		return tonumber(k) or Keysym[k] or k
	end
	for k,v in pairs(Binding.dict_name) do
		local keys = self.controls:get(k)
		if keys then
			local key1,key2 = string.match(keys, "([a-zA-Z0-9]*)[ \t]*([a-zA-Z0-9]*)")
			key1 = translate(key1)
			key2 = translate(key2)
			if key1 then v.key1 = key1 end
			if key2 then v.key2 = key2 end
		end
	end
end

--- Saves controls to the configuration file.
-- @param self Client.
Client.save_controls = function(self)
	local translate = function(k)
		if not k then return "none" end
		return Keycode[k] or tostring(k)
	end
	for k,v in pairs(Binding.dict_name) do
		local key1 = translate(v.key1)
		local key2 = translate(v.key2)
		self.controls:set(k, key1 .. " " .. key2)
	end
	self.controls:save()
end

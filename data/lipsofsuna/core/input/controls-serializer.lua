--- Controls loading and saving.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.input.controls_serializer
-- @alias ControlsSerializer

local Class = require("system/class")
local ConfigFile = require("system/config-file")
local Keycode = require("system/keycode")
local Keysym = require("system/keysym")

--- Controls loading and saving.
-- @type ControlsSerializer
local ControlsSerializer = Class("ControlsSerializer")

--- Initializes the controls serializer.
-- @param clss Input class.
-- @return Input.
ControlsSerializer.new = function(clss)
	local self = Class.new(clss)
	self.data = {}
	self.data.controls = ConfigFile{name = "controls.cfg"}
	self.data.version = "A"
	return self
end

--- Loads controls from the configuration file.
-- @param self ControlsSerializer.
-- @param bindings Bindings to load.
ControlsSerializer.load = function(self, bindings)
	if self.data.controls:get("version") ~= self.data.version then return end
	local translate = function(k)
		if not k or k == "none" then return end
		return tonumber(k) or Keysym[k] or k
	end
	for k,v in pairs(bindings.dict_name) do
		local keys = self.data.controls:get(k)
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
-- @param self ControlsSerializer.
-- @param bindings Bindings to save.
ControlsSerializer.save = function(self, bindings)
	local translate = function(k)
		if not k then return "none" end
		return Keycode[k] or tostring(k)
	end
	for k,v in pairs(bindings.dict_name) do
		local key1 = translate(v.key1)
		local key2 = translate(v.key2)
		self.data.controls:set(k, key1 .. " " .. key2)
	end
	self.data.controls:set("version", self.data.version)
	self.data.controls:save()
end

return ControlsSerializer

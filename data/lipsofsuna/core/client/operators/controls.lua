local Class = require("system/class")
local ConfigFile = require("system/config-file")

Operators.controls = Class("ControlsOperator")
Operators.controls.data = {}

--- Initializes the controls operator.
--
-- Context: Any.
--
-- @param self Controls operator.
Operators.controls.init = function(self)
	self.data.controls = ConfigFile{name = "controls.cfg"}
	self.data.version = "A"
end

--- Loads controls from the configuration file.
--
-- Context: Controls must have been initialized.
--
-- @param self Controls operator.
Operators.controls.load = function(self)
	if self.data.controls:get("version") ~= self.data.version then return end
	local translate = function(k)
		if not k or k == "none" then return end
		return tonumber(k) or Keysym[k] or k
	end
	for k,v in pairs(Client.bindings.dict_name) do
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
--
-- Context: Controls must have been initialized.
--
-- @param self Controls operator.
Operators.controls.save = function(self)
	local translate = function(k)
		if not k then return "none" end
		return Keycode[k] or tostring(k)
	end
	for k,v in pairs(Client.bindings.dict_name) do
		local key1 = translate(v.key1)
		local key2 = translate(v.key2)
		self.data.controls:set(k, key1 .. " " .. key2)
	end
	self.data.controls:set("version", self.data.version)
	self.data.controls:save()
end

local Class = require("system/class")
local UiEntry = require("ui/widgets/entry")
local UiScrollFloat = require("ui/widgets/scrollfloat")
local UiScrollInteger = require("ui/widgets/scrollinteger")
local UiToggle = require("ui/widgets/toggle")
local UiWidget = require("ui/widgets/widget")

------------------------------------------------------------------------------

local UiConfigOptionBool = Class("UiConfigOptionBool", UiToggle)

UiConfigOptionBool.new = function(clss, option, changed)
	local opt = Client.options.config_keys[option]
	local value = Client.options[option]
	-- Create the widget.
	local self = UiToggle.new(clss, opt[1])
	self.key = option
	self.value = value
	-- Report the initial value.
	self.changed_cb = changed
	self:changed()
	return self
end

UiConfigOptionBool.changed = function(self)
	Client.options[self.key] = self.value
	Client.options:save()
	if self.changed_cb then self.changed_cb(self.key, self.value) end
end

------------------------------------------------------------------------------

local UiConfigOptionFloat = Class("UiconfigOptionFloat", UiScrollFloat)

UiConfigOptionFloat.new = function(clss, option, changed)
	local opt = Client.options.config_keys[option]
	local value = Client.options[option]
	-- Create the widget.
	local self = UiScrollFloat.new(clss, opt[1], opt[3], opt[4], value)
	self.key = option
	-- Report the initial value.
	self.changed_cb = changed
	self:changed()
	return self
end

UiConfigOptionFloat.changed = function(self)
	Client.options[self.key] = self.value
	Client.options:save()
	if self.changed_cb then self.changed_cb(self.key, self.value) end
end

------------------------------------------------------------------------------

local UiConfigOptionInt = Class("UiConfigOptionInt", UiScrollInteger)

UiConfigOptionInt.new = function(clss, option, changed)
	local opt = Client.options.config_keys[option]
	local value = Client.options[option]
	-- Create the widget.
	local self = UiScrollInteger.new(clss, opt[1], opt[3], opt[4], value)
	self.key = option
	-- Report the initial value.
	self.changed_cb = changed
	self:changed()
	return self
end

UiConfigOptionInt.changed = function(self)
	Client.options[self.key] = self.value
	Client.options:save()
	if self.changed_cb then self.changed_cb(self.key, self.value) end
end

------------------------------------------------------------------------------

local UiConfigOptionPow = Class("UiConfigOptionPow", UiScrollInteger)

UiConfigOptionPow.new = function(clss, option, changed)
	local opt = Client.options.config_keys[option]
	local value_exp = Client.options[option]
	local value_lin = math.floor(math.log(value_exp)/math.log(2) + 0.5)
	-- Create the widget.
	local self = UiScrollInteger.new(clss, opt[1], opt[3], opt[4], value_lin)
	self.key = option
	-- Report the initial value.
	self.changed_cb = changed
	self:changed()
	return self
end

UiConfigOptionPow.changed = function(self)
	-- Calculate the real value.
	self.value_pow = 2 ^ self.value
	self.text = tostring(self.value_pow) .. "x"
	-- Update the option.
	Client.options[self.key] = self.value_pow
	Client.options:save()
	if self.changed_cb then self.changed_cb(self.key, self.value_pow) end
end

------------------------------------------------------------------------------

local UiConfigOptionString = Class("UiConfigOptionString", UiEntry)

UiConfigOptionString.new = function(clss, option, changed)
	local opt = Client.options.config_keys[option]
	local value = Client.options[option]
	-- Create the widget.
	local self = UiEntry.new(clss, opt[1])
	self.key = option
	self.value = value
	-- Report the initial value.
	self.changed_cb = changed
	self:changed()
	return self
end

UiConfigOptionString.changed = function(self)
	Client.options[self.key] = self.value
	Client.options:save()
	if self.changed_cb then self.changed_cb(self.key, self.value) end
end

------------------------------------------------------------------------------

local UiConfigOption = Class("UiConfigOption", UiWidget)

UiConfigOption.new = function(clss, option, changed)
	local opt = Client.options.config_keys[option]
	if not opt then return end
	if opt[2] == "bool" then
		return UiConfigOptionBool(option, changed)
	elseif opt[2] == "float" then
		return UiConfigOptionFloat(option, changed)
	elseif opt[2] == "int" then
		return UiConfigOptionInt(option, changed)
	elseif opt[2] == "pow" then
		return UiConfigOptionPow(option, changed)
	elseif opt[2] == "string" then
		return UiConfigOptionString(option, changed)
	end
end

return UiConfigOption

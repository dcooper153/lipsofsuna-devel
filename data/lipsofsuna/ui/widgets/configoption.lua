local Class = require("system/class")
require(Mod.path .. "entry")
require(Mod.path .. "scrollfloat")
require(Mod.path .. "scrollinteger")
require(Mod.path .. "toggle")
require(Mod.path .. "widget")

Widgets.Uiconfigoption = Class("Uiconfigoption", Widgets.Uiwidget)

Widgets.Uiconfigoption.new = function(clss, option, changed)
	local opt = Client.options.config_keys[option]
	if not opt then return end
	if opt[2] == "bool" then
		return Widgets.Uiconfigoptionbool(option, changed)
	elseif opt[2] == "float" then
		return Widgets.Uiconfigoptionfloat(option, changed)
	elseif opt[2] == "int" then
		return Widgets.Uiconfigoptionint(option, changed)
	elseif opt[2] == "pow" then
		return Widgets.Uiconfigoptionpow(option, changed)
	elseif opt[2] == "string" then
		return Widgets.Uiconfigoptionstring(option, changed)
	end
end

------------------------------------------------------------------------------

Widgets.Uiconfigoptionbool = Class("Uiconfigoptionbool", Widgets.Uitoggle)

Widgets.Uiconfigoptionbool.new = function(clss, option, changed)
	local opt = Client.options.config_keys[option]
	local value = Client.options[option]
	-- Create the widget.
	local self = Widgets.Uitoggle.new(clss, opt[1])
	self.key = option
	self.value = value
	-- Report the initial value.
	self.changed_cb = changed
	self:changed()
	return self
end

Widgets.Uiconfigoptionbool.changed = function(self)
	Client.options[self.key] = self.value
	Client.options:save()
	if self.changed_cb then self.changed_cb(self.key, self.value) end
end

------------------------------------------------------------------------------

Widgets.Uiconfigoptionfloat = Class("Uiconfigoptionfloat", Widgets.Uiscrollfloat)

Widgets.Uiconfigoptionfloat.new = function(clss, option, changed)
	local opt = Client.options.config_keys[option]
	local value = Client.options[option]
	-- Create the widget.
	local self = Widgets.Uiscrollfloat.new(clss, opt[1], opt[3], opt[4], value)
	self.key = option
	-- Report the initial value.
	self.changed_cb = changed
	self:changed()
	return self
end

Widgets.Uiconfigoptionfloat.changed = function(self)
	Client.options[self.key] = self.value
	Client.options:save()
	if self.changed_cb then self.changed_cb(self.key, self.value) end
end

------------------------------------------------------------------------------

Widgets.Uiconfigoptionint = Class("Uiconfigoptionint", Widgets.Uiscrollinteger)

Widgets.Uiconfigoptionint.new = function(clss, option, changed)
	local opt = Client.options.config_keys[option]
	local value = Client.options[option]
	-- Create the widget.
	local self = Widgets.Uiscrollinteger.new(clss, opt[1], opt[3], opt[4], value)
	self.key = option
	-- Report the initial value.
	self.changed_cb = changed
	self:changed()
	return self
end

Widgets.Uiconfigoptionint.changed = function(self)
	Client.options[self.key] = self.value
	Client.options:save()
	if self.changed_cb then self.changed_cb(self.key, self.value) end
end

------------------------------------------------------------------------------

Widgets.Uiconfigoptionpow = Class("Uiconfigoptionpow", Widgets.Uiscrollinteger)

Widgets.Uiconfigoptionpow.new = function(clss, option, changed)
	local opt = Client.options.config_keys[option]
	local value_exp = Client.options[option]
	local value_lin = math.floor(math.log(value_exp)/math.log(2) + 0.5)
	-- Create the widget.
	local self = Widgets.Uiscrollinteger.new(clss, opt[1], opt[3], opt[4], value_lin)
	self.key = option
	-- Report the initial value.
	self.changed_cb = changed
	self:changed()
	return self
end

Widgets.Uiconfigoptionpow.changed = function(self)
	-- Calculate the real value.
	self.value_pow = 2 ^ self.value
	self.text = tostring(self.value_pow) .. "x"
	-- Update the option.
	Client.options[self.key] = self.value_pow
	Client.options:save()
	if self.changed_cb then self.changed_cb(self.key, self.value_pow) end
end

------------------------------------------------------------------------------

Widgets.Uiconfigoptionstring = Class("Uiconfigoptionstring", Widgets.Uientry)

Widgets.Uiconfigoptionstring.new = function(clss, option, changed)
	local opt = Client.options.config_keys[option]
	local value = Client.options[option]
	-- Create the widget.
	local self = Widgets.Uientry.new(clss, opt[1])
	self.key = option
	self.value = value
	-- Report the initial value.
	self.changed_cb = changed
	self:changed()
	return self
end

Widgets.Uiconfigoptionstring.changed = function(self)
	Client.options[self.key] = self.value
	Client.options:save()
	if self.changed_cb then self.changed_cb(self.key, self.value) end
end

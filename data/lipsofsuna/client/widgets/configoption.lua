Widgets.ConfigOption = Class(Widget)
Widgets.ConfigOption.class_name = "Widgets.ConfigOption"

Widgets.ConfigOption.new = function(clss, option, changed)
	-- Find the affected configuration option.
	local opt = Client.options.config_keys[option]
	if not opt then return end
	-- Create the widget.
	local self = Widget.new(clss)
	self.type = opt[2]
	self.key = option
	self.value = Client.options[option]
	self:set_request{width = 300, height = 20}
	-- Create the label widget.
	self.label = Widgets.Label{text = opt[1], valign = 0.5}
	self.label:set_request{width = 150, height = 20}
	self:add_child(self.label)
	-- Create the child widget.
	if self.type == "bool" then
		self.child = Widgets.Toggle{active = self.value}
		self.child.pressed = function(widget)
			self.value = not widget.active
			widget.active = self.value
			self:changed()
		end
		self:add_child(self.child)
	elseif self.type == "float" then
		self.child = Widgets.Progress{min = opt[3], max = opt[4], value = self.value}
		self.child.pressed = function(widget)
			local v = widget:get_value_at(Program.cursor_position)
			self.value = v
			widget.value = v
			self:changed()
		end
	elseif self.type == "int" then
		self.child = Widgets.Progress{min = opt[3], max = opt[4], text = tostring(self.value), value = self.value}
		self.child.pressed = function(widget)
			local v = widget:get_value_at(Program.cursor_position)
			self.value = math.floor(v + 0.5)
			widget.value = self.value
			widget.text = tostring(self.value)
			self:changed()
		end
	elseif self.type == "pow" then
		local exp = (self.value < 1) and 0 or math.floor(math.log(self.value)/math.log(2) + 0.5)
		self.child = Widgets.Progress{min = opt[3], max = opt[4], text = tostring(self.value) .. "x", value = exp}
		self.child.pressed = function(widget)
			local v = widget:get_value_at(Program.cursor_position)
			widget.value = math.floor(v + 0.5)
			self.value = 2 ^ widget.value
			widget.text = tostring(self.value) .. "x"
			self:changed()
		end
	end
	if self.child then
		self.child.offset = Vector(150, 0)
		self.child:set_request{width = 150, height = 20}
		self:add_child(self.child)
	end
	-- Report the initial value.
	self.changed_cb = changed
	self:changed()
	return self
end

Widgets.ConfigOption.changed = function(self)
	Client.options[self.key] = self.value
	Client.options:save()
	if self.changed_cb then self.changed_cb(self.key, self.value) end
end

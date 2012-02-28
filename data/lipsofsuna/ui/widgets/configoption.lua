require(Mod.path .. "widget")

Widgets.Uiconfigoption = Class(Widgets.Uiwidget)
Widgets.Uiconfigoption.class_name = "Widgets.Uiconfigoption"

Widgets.Uiconfigoption.new = function(clss, option, changed)
	-- Find the affected configuration option.
	local opt = Client.options.config_keys[option]
	if not opt then return end
	-- Create the widget.
	local self = Widgets.Uiwidget.new(clss, nil, opt[1])
	self.type = opt[2]
	self.key = option
	self.value = Client.options[option]
	-- Create the child widget.
	if self.type == "bool" then
		self.hint = "$A: Toggle\n$$B\n$$U\n$$D"
		self.child = Widgets.Toggle{active = self.value}
		self.child.pressed = function(widget)
			self.value = not widget.active
			widget.active = self.value
			self:changed()
		end
		self:add_child(self.child)
	elseif self.type == "float" then
		self.hint = "$A: Increment\n$B: Decrement\n$$U\n$$D"
		self.min = opt[3]
		self.max = opt[4]
		self.step = (opt[4] - opt[3]) / 10
		self.child = Widgets.Progress{min = opt[3], max = opt[4], value = self.value}
		self.child.pressed = function(widget)
			local v = widget:get_value_at(Program.cursor_position)
			self.value = v
			widget.value = v
			self:changed()
		end
	elseif self.type == "int" then
		self.hint = "$A: Increment\n$B: Decrement\n$$U\n$$D"
		self.min = opt[3]
		self.max = opt[4]
		self.child = Widgets.Progress{min = opt[3], max = opt[4], text = tostring(self.value), value = self.value}
		self.child.pressed = function(widget)
			local v = widget:get_value_at(Program.cursor_position)
			self.value = math.floor(v + 0.5)
			widget.value = self.value
			widget.text = tostring(self.value)
			self:changed()
		end
	elseif self.type == "pow" then
		self.hint = "$A: Increment\n$B: Decrement\n$$U\n$$D"
		self.min = 2^opt[3]
		self.max = 2^opt[4]
		local exp = math.floor(math.log(self.value)/math.log(2) + 0.5)
		self.child = Widgets.Progress{min = opt[3], max = opt[4], text = tostring(self.value) .. "x", value = exp}
		self.child.pressed = function(widget)
			local v = widget:get_value_at(Program.cursor_position)
			widget.value = math.floor(v + 0.5)
			self.value = 2 ^ widget.value
			widget.text = tostring(self.value) .. "x"
			self:changed()
		end
	elseif self.type == "string" then
		self.hint = "$A: Edit\n$$B\n$$U\n$$D"
		self.child = Widgets.Entry{text = self.value}
		self.child.changed = function(w)
			self.value = w.text
			self:changed()
		end
	end
	-- Report the initial value.
	self.changed_cb = changed
	self:changed()
	return self
end

Widgets.Uiconfigoption.apply = function(self)
	if self.type == "bool" then
		self.value = not self.value
		self.child.active = self.value
		self:changed()
	elseif self.type == "string" then
		self.input_mode = true
		self.hint = "[RETURN]: End editing"
	elseif self.type == "int" then
		if self.value == self.max then return end
		self.value = self.value + 1
		self.child.text = tostring(self.value)
		self.child.value = self.value
		self:changed()
	elseif self.type == "float" then
		if self.value == self.max then return end
		self.value = math.min(self.value + self.step, self.max)
		self.child.value = self.value
		self:changed()
	elseif self.type == "pow" then
		if self.value == self.max then return end
		self.value = math.floor(self.value * 2 + 0.5)
		self.child.value = math.floor(math.log(self.value)/math.log(2) + 0.5)
		self.child.text = tostring(self.value) .. "x"
		self:changed()
	end
end

Widgets.Uiconfigoption.apply_back = function(self)
	if self.type == "int" then
		if self.value == self.min then return true end
		self.value = self.value - 1
		self.child.text = tostring(self.value)
		self.child.value = self.value
		self:changed()
	elseif self.type == "float" then
		if self.value == self.min then return true end
		self.value = math.max(self.value - self.step, self.min)
		self.child.value = self.value
		self:changed()
	elseif self.type == "pow" then
		if self.value == self.min then return true end
		self.value = math.floor(self.value / 2 + 0.5)
		self.child.value = math.floor(math.log(self.value)/math.log(2) + 0.5)
		self.child.text = tostring(self.value) .. "x"
		self:changed()
	else
		return true
	end
end

Widgets.Uiconfigoption.changed = function(self)
	Client.options[self.key] = self.value
	Client.options:save()
	if self.changed_cb then self.changed_cb(self.key, self.value) end
end

Widgets.Uiconfigoption.handle_event = function(self, args)
	if not self.input_mode then return true end
	if args.type ~= "keypress" then return true end
	if args.code == Keysym.ESCAPE or args.code == Keysym.RETURN then
		self.input_mode = false
		self.hint = "$A: Edit\n$$B\n$$U\n$$D"
	else
		self.child:event(args)
	end
end

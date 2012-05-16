require(Mod.path .. "widget")

Widgets.Uiscrollfloat = Class(Widgets.Uiwidget)
Widgets.Uiscrollfloat.class_name = "Widgets.Uiscrollfloat"

Widgets.Uiscrollfloat.new = function(clss, label, min, max, value, changed)
	local self = Widgets.Uiwidget.new(clss, nil, label)
	self.min = min
	self.max = max
	self.value = value
	self.changed = changed
	self.step = (max - min) / 10
	self.hint = "$A: Edit\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiscrollfloat.apply = function(self)
	if not self.input_mode then
		self.input_mode = true
		self.hint = "$A: Increment\n$B: Decrement\n$$U\n$$D"
		self.need_repaint = true
		Effect:play_global("uislider1")
		return
	end
	if self.value == self.max then return end
	self.value = math.min(self.value + self.step, self.max)
	self.need_repaint = true
	self:update_text()
	self:changed()
	Effect:play_global("uislider1")
end

Widgets.Uiscrollfloat.apply_back = function(self)
	if not self.input_mode then
		self.need_repaint = true
		Effect:play_global("uislider1")
		return Widgets.Uiwidget.apply_back(self)
	end
	if self.value == self.min then return true end
	self.value = math.max(self.value - self.step, self.min)
	self.need_repaint = true
	self:update_text()
	self:changed()
	Effect:play_global("uislider1")
end

Widgets.Uiscrollfloat.changed = function(self)
end

Widgets.Uiscrollfloat.handle_event = function(self, args)
	local handled
	if self.input_mode then
		-- Scroll if a horizontal menu key was pressed.
		if args.type ~= "keyrelease" then
			local a = {}
			for k,v in pairs(args) do a[k] = v end
			if args.type == "keyrepeat" then
				a.type = "keypress"
			end
			local action1 = Binding.dict_name["menu apply"]
			local action2 = Binding.dict_name["menu back"]
			if (action1 and action1:get_event_response(a) ~= nil) then
				self:apply()
				handled = true
			elseif (action2 and action2:get_event_response(a) ~= nil) then
				self:apply_back()
				handled = true
			end
		end
		-- Stop editing if a vertical menu key.
		local action3 = Binding.dict_name["menu up"]
		local action4 = Binding.dict_name["menu down"]
		if (action3 and action3:get_event_response(args) ~= nil) or
		   (action4 and action4:get_event_response(args) ~= nil) then
			self.hint = "$A: Edit\n$$B\n$$U\n$$D"
			self.input_mode = nil
		end
	end
	if not Ui.pointer_grab then
		if args.type == "mousepress" then
			local cx = Program.cursor_position.x
			if args.button == 1 then
				if cx < self.x + Theme.width_label_1 + Theme.width_slider_button_1 then
					self.value = math.max(self.value - self.step, self.min)
					self.need_repaint = true
					self:update_text()
					self:changed()
				elseif cx >= self.x + self.size.x - Theme.width_slider_button_1 then
					self.value = math.min(self.value + self.step, self.max)
					self.need_repaint = true
					self:update_text()
					self:changed()
				else
					self:set_value_at(cx)
				end
			end
			return
		elseif args.type == "mousemotion" then
			if math.mod(Program.mouse_button_state, 2) == 1 then
				self:set_value_at(args.x)
			end
			return
		end
	end
	if handled then return end
	return Widgets.Uiwidget.handle_event(self, args)
end

Widgets.Uiscrollfloat.rebuild_canvas = function(self)
	local w = self.size.x - Theme.width_label_1 - 5
	local h = self.size.y - 10
	local v = (self.value - self.min) / (self.max - self.min)
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the background.
	Theme:draw_slider(self, v,
		Theme.width_label_1, 5, w, h,
		self.focused, self.input_mode)
	-- Add the label of the bar.
	if self.text then
		self:canvas_text{
			dest_position = {Theme.width_label_1,Theme.text_pad_1+5},
			dest_size = {w,h},
			text = self.text,
			text_alignment = {0.5,0.5},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_1}
	end
end

Widgets.Uiscrollfloat.update_text = function(self)
end

Widgets.Uiscrollfloat.get_value_at = function(self, x)
	local pos = math.max(0, x - self.x - Theme.width_label_1 - Theme.width_slider_button_1)
	local frac = math.min(1, pos / (self.size.x - Theme.width_label_1 - Theme.width_slider_button_1 * 2 - 5))
	return frac * (self.max - self.min) + self.min
end

Widgets.Uiscrollfloat.set_value_at = function(self, x)
	self.value = self:get_value_at(x)
	self.need_repaint = true
	self:update_text()
	self:changed()
end

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
		return
	end
	if self.value == self.max then return end
	self.value = math.min(self.value + self.step, self.max)
	self.need_repaint = true
	self:changed()
end

Widgets.Uiscrollfloat.apply_back = function(self)
	if not self.input_mode then
		return Widgets.Uiwidget.apply_back(self)
	end
	if self.value == self.min then return true end
	self.value = math.max(self.value - self.step, self.min)
	self.need_repaint = true
	self:changed()
end

Widgets.Uiscrollfloat.changed = function(self)
end

Widgets.Uiscrollfloat.handle_event = function(self, args)
	if self.input_mode then
		local action1 = Action.dict_name["menu up"]
		local action2 = Action.dict_name["menu down"]
		if (action1 and action1:get_event_response(args) ~= nil) or
		   (action2 and action2:get_event_response(args) ~= nil) then
			self.hint = "$A: Edit\n$$B\n$$U\n$$D"
			self.input_mode = nil
		end
	end
	if not Ui.pointer_grab then
		if args.type == "mousepress" then
			if args.button == 2 then
				self:set_value_at(Program.cursor_position.x)
			end
			return
		elseif args.type == "mousemotion" then
			if math.mod(Program.mouse_button_state, 2) == 1 then
				self:set_value_at(args.x)
			end
			return
		end
	end
	return Widgets.Uiwidget.handle_event(self, args)
end

Widgets.Uiscrollfloat.rebuild_canvas = function(self)
	local w = self.size.x - 155
	local h = self.size.y - 10
	local v = (self.value - self.min) / (self.max - self.min)
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the background.
	self:canvas_image{
		dest_position = {150,5},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = {450,375},
		source_tiling = {34,120,34,6,15,7}}
	-- Add the highlighted area.
	if v > 0 then
		self:canvas_image{
			dest_clip = {0,0,v*w,h},
			dest_position = {150,5},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {450,405},
			source_tiling = {34,120,34,6,15,7}}
	end
	-- Add the label of the bar.
	if self.text then
		self:canvas_text{
			dest_position = {150,5},
			dest_size = {w,h},
			text = self.text,
			text_alignment = {0.5,0.5},
			text_color = {1,1,1,1},
			text_font = "tiny"}
	end
end

Widgets.Uiscrollfloat.get_value_at = function(self, x)
	local pos = math.max(0, x - self.x - 150)
	local frac = math.min(1, pos / (self.size.x - 155))
	return frac * (self.max - self.min) + self.min
end

Widgets.Uiscrollfloat.set_value_at = function(self, x)
	self.value = self:get_value_at(x)
	self.need_repaint = true
	self:changed()
end

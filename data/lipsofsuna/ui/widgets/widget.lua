require "system/widget"

Widgets.Uiwidget = Class(Widget)
Widgets.Uiwidget.class_name = "Widgets.Uiwidget"

Widgets.Uiwidget.new = function(clss, child, label)
	-- Create the widget.
	local self = Widget.new(clss)
	self.size = Vector()
	self.label = label
	self.need_reshape = true
	self.need_repaint = true
	-- Add the child widget.
	self.child = child
	return self
end

Widgets.Uiwidget.apply = function(self)
	if self.child and self.child.pressed then
		self.child:pressed()
	end
end

Widgets.Uiwidget.apply_back = function(self)
	return true
end

Widgets.Uiwidget.handle_event = function(self, args)
	if Ui.pointer_grab then return true end
	if args.type ~= "mousepress" then return true end
	self:apply()
end

Widgets.Uiwidget.update = function(self, secs)
	-- Update the size.
	if self.need_reshape then
		self.need_reshape = nil
		local size = self:rebuild_size()
		if self.size.x ~= size.x or self.size.y ~= size.y then
			self:set_request{width = size.x, height = size.y}
			self.size = size
			Ui.need_relayout = true
		end
	end
	-- Update the graphics.
	if self.need_repaint then
		self.need_repaint = nil
		self:canvas_clear()
		self:rebuild_canvas()
	end
end

Widgets.Uiwidget.rebuild_size = function(self)
	-- Set the default size.
	local size = Vector(Theme.width_widget_1, Theme.height_widget_1)
	-- Resize to fit the label.
	if self.label then
		local w,h = Program:measure_text(Theme.text_font_1, self.label, Theme.width_label_1-5)
		if h then size.y = math.max(size.y, h) end
	end
	-- Resize to fit the child.
	if self.child then
		size.y = math.max(size.y, self.child.height_request or 0)
	end
	return size
end

Widgets.Uiwidget.rebuild_canvas = function(self)
	-- Add the background.
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {self.size.x,self.size.y},
		source_image = "widgets2",
		source_position = self.focused and {0,100} or {0,0},
		source_tiling = {7,86,7,7,86,7}}
	-- Add the label.
	if self.label then
		self:canvas_text{
			dest_position = {Theme.text_pad_1,Theme.text_pad_1},
			dest_size = {Theme.width_label_1-5,self.size.y},
			text = self.label,
			text_alignment = {0,0.5},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_1}
	end
end

Widgets.Uiwidget:add_getters{
	child = function(self) return rawget(self, "__child") end,
	focused = function(self) return rawget(self, "__focused") end,
	help = function(self) return rawget(self, "__help") end,
	hint = function(self) return rawget(self, "__hint") end}

Widgets.Uiwidget:add_setters{
	child = function(self, v)
		rawset(self, "__child", v)
		if v then
			if self.label then
				v.offset = Vector(150, 5)
				v:set_request{width = 150, height = 20}
			else
				v.offset = Vector(10, 5)
				v:set_request{width = 300, height = 20}
			end
			self:add_child(v)
			self.need_reshape = true
		end
	end,
	focused = function(self, v)
		if self.focused == v then return end
		rawset(self, "__focused", v)
		self.need_repaint = true
	end,
	help = function(self, v)
		if self.help == v then return end
		rawset(self, "__help", v)
		if self.focused then Ui:update_help() end
	end,
	hint = function(self, v)
		if self.hint == v then return end
		rawset(self, "__hint", v)
		if self.focused then Ui:update_help() end
	end}

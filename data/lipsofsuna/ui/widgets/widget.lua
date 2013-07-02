local Class = require("system/class")
local Interpolation = require("system/math/interpolation")
local Widget = require("system/widget")

local ipol_time = 0.2
local ipol_path_a = {0,0.1,0.2,0.5,0.8,1,1}
local ipol_path_x = {-1,-0.5,-0.25,-0.0125,0,0,0}

Widgets.Uiwidget = Class("Uiwidget", Widget)

Widgets.Uiwidget.new = function(clss, label)
	-- Create the widget.
	local self = Widget.new(clss)
	self.__life = 0
	self.__offset = Vector()
	self.size = Vector()
	self.label = label
	self.need_reshape = true
	self.need_repaint = true
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
	if Ui:get_pointer_grab() then return true end
	if args.type ~= "mousepress" then return true end
	self:apply()
end

Widgets.Uiwidget.update = function(self, secs)
	-- Update the size.
	if self.need_reshape then
		self.need_reshape = nil
		local size = self:rebuild_size()
		if self.size.x ~= size.x or self.size.y ~= size.y then
			self:set_request(size.x, size.y)
			self.size = size
			Ui.need_relayout = true
		end
	end
	-- Update the offset.
	if self.__need_reoffset or self.__life then
		self.__need_reoffset = nil
		local alpha = 1
		local offset = self.__offset
		if self.__life then
			self.__life = self.__life + secs
			if self.__life < ipol_time then
				alpha = Interpolation:interpolate_samples_cubic_1d(self.__life / ipol_time * 6, ipol_path_a)
				local x = Interpolation:interpolate_samples_cubic_1d(self.__life / ipol_time * 6, ipol_path_x)
				offset = offset:copy():add_xyz(x * 500)
			else
				self.__life = nil
			end
		end
		self:set_alpha(alpha)
		Widget.set_offset(self, offset)
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
		local w,h = self.child:get_request()
		size.y = math.max(size.y, h or 0)
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

Widgets.Uiwidget.set_focused = function(self, v)
	if self.focused == v then return end
	self.focused = v
	self.need_repaint = true
end

Widgets.Uiwidget.get_help = function(self)
	return self.help
end

Widgets.Uiwidget.set_help = function(self, v)
	if self.help == v then return end
	self.help = v
	if self.focused then Ui:update_help() end
end

Widgets.Uiwidget.get_hint = function(self)
	return self.hint or "$$A\n$$B\n$$U\n$$D"
end

Widgets.Uiwidget.set_hint = function(self, v)
	if self.hint == v then return end
	self.hint = v
	if self.focused then Ui:update_help() end
end

--- Sets the interpolation priority of the widget.
-- @param self Uiwidget.
-- @param v Number.
Widgets.Uiwidget.set_show_priority = function(self, v)
	if not self.__life then return end
	if Client.options.ui_animations then
		self.__life = -0.05 * math.log(v + 0.2)
	else
		self.__life = nil
	end
	self.__need_reoffset = true
end

Widgets.Uiwidget.get_offset = function(self)
	return self.__offset
end

Widgets.Uiwidget.set_offset = function(self, v)
	self.__offset = v
	self.__need_reoffset = true
end

return Widgets.Uiwidget

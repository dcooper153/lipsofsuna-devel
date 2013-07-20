local Class = require("system/class")
local Interpolation = require("system/math/interpolation")
local Widget = require("system/widget")

local ipol_time = 0.2
local ipol_path_a = {0,0.1,0.2,0.5,0.8,1,1}
local ipol_path_x = {-1,-0.5,-0.25,-0.0125,0,0,0}

local UiWidget = Class("Uiwidget", Widget)

-- FIXME
Widgets.Uiwidget = UiWidget

UiWidget.new = function(clss, label)
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

UiWidget.apply = function(self)
	if self.child and self.child.pressed then
		self.child:pressed()
	end
end

UiWidget.apply_back = function(self)
	return true
end

UiWidget.handle_event = function(self, args)
	if Ui:get_pointer_grab() then return true end
	if args.type ~= "mousepress" then return true end
	local w = self:get_widget_by_point(Vector(args.x, args.y))
	if not w then return true end
	return w:apply()
end

UiWidget.update = function(self, secs)
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

UiWidget.rebuild_size = function(self)
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

UiWidget.rebuild_canvas = function(self)
	-- Add the background.
	Theme:draw_base(self, 0, 0, self.size.x, self.size.y, self.focused)
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

--- Sets the focus state of the widget.
-- @param self UiSelector.
-- @param value True to focus. False otherwise.
-- @return True if the focus changed. False if the widget rejected the change.
UiWidget.set_focused = function(self, v)
	if self.focused == v then return true end
	self.focused = v
	self.need_repaint = true
	return true
end

UiWidget.get_help = function(self)
	return self.help
end

UiWidget.set_help = function(self, v)
	if self.help == v then return end
	self.help = v
	if self.focused then Ui:update_help() end
end

UiWidget.get_hint = function(self)
	return self.hint or "$$A\n$$B\n$$U\n$$D"
end

UiWidget.set_hint = function(self, v)
	if self.hint == v then return end
	self.hint = v
	if self.focused then Ui:update_help() end
end

--- Sets the interpolation priority of the widget.
-- @param self Uiwidget.
-- @param v Number.
UiWidget.set_show_priority = function(self, v)
	if not self.__life then return end
	if Client.options.ui_animations then
		self.__life = -0.05 * math.log(v + 0.2)
	else
		self.__life = nil
	end
	self.__need_reoffset = true
end

UiWidget.get_offset = function(self)
	return self.__offset
end

UiWidget.set_offset = function(self, v)
	self.__offset = v
	self.__need_reoffset = true
end

--- Finds the widget that encloses the point.<br/>
--
-- Returns the first widget whose rectangle encloses the given point.
-- If the filter funtion is given, the first matching widget for which the
-- filter returns true will be returned. Other widgets are skipped.
--
-- @param self UiWidget.
-- @param point Point in screen space.
-- @param filter Optional filter function.
-- @return Widget if found. Nil otherwise.
UiWidget.get_widget_by_point = function(self, point, filter)
	-- Check for visibility.
	if not self:get_visible() then return end
	-- Check for X coordinate.
	local x,w = self:get_x(),self:get_width()
	if point.x < x or x + w <= point.x then return end
	-- Check for Y coordinate.
	local y,h = self:get_y(),self:get_height()
	if point.y < y or y + h <= point.y then return end
	-- Apply filtering.
	if not filter or filter(self) then return self end
end

return UiWidget

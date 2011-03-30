Widgets.Cursor = Class(Widget)

Widgets.Cursor.setter = function(self, key, value)
	if key == "cursor" then
		if self.cursor ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "widget" then
		if self.widget ~= value then
			Widget.setter(self, key, value)
			self:set_child{col = 1, row = 1, widget = value}
		end
	else
		Widget.setter(self, key, value)
	end
end

Widgets.Cursor.new = function(clss, cursor)
	local self = Widget.new(clss, {cursor = cursor, cols = 1, rows = 1})
	return self
end

Widgets.Cursor.update = function(self)
	-- Update position.
	local c = Client.cursor_pos
	self.floating = not Client.moving
	self.x = c.x + 1
	self.y = c.y + 1
	-- Show tooltips.
	if self.tooltip then
		self.tooltip.floating = false
		self.tooltip = nil
	end
	if self.widget then return end
	if self.floating then
		local w = Widgets:find_handler_widget("tooltip")
		if w and w.tooltip then
			self.tooltip = w.tooltip
			w.tooltip:popup(c + Vector(5,5))
		end
	end
end

Widgets.Cursor.reshaped = function(self)
	local icon = self.icon
	local cursor = self.cursor
	self:set_request{
		internal = true,
		height = cursor and cursor.size[2] or 16,
		width = cursor and cursor.size[1] or 16}
	self:canvas_clear()
	if self.cursor then
		self:canvas_image{
			dest_position = {0,0},
			dest_size = cursor.size,
			source_image = cursor.image,
			source_position = cursor.offset,
			source_tiling = {0,cursor.size[1],0,0,cursor.size[1],0}}
	end
	self:canvas_compile()
end

Widgets.Scrollbar = Class(Widget)
Widgets.Scrollbar.class_name = "Widgets.Scrollbar"

Widgets.Scrollbar.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.max = 0
	self.page = 1
	self.step = 1
	self.scroll_offset = 1
	return self
end

Widgets.Scrollbar.changed = function(self, value)
end

Widgets.Scrollbar.grabbed = function(self, value)
end

Widgets.Scrollbar.get_value_at = function(self, point)
	if self.max == 0 then return end
	local p = point - Vector(self.x, self.y)
	local v = p.y / self.height * self.max
	return math.floor(v + 0.5)
end

Widgets.Scrollbar.set_value_at = function(self, point)
	-- Ignore if the scroll range is empty.
	if self.max <= self.page then return end
	-- Clamp clicks on the buttons.
	local c = point - Vector(self.x, self.y)
	c.y = math.max(c.y, 21)
	c.y = math.min(c.y, self.height - 21 - 1)
	-- Scroll to the cursor.
	local v = self:get_value_at(point)
	if not v then return end
	self.scroll_offset = math.max(0, math.min(v - math.floor(self.page/2), self.max - self.page))
	self:reshaped()
	self:changed(self.scroll_offset)
	return true
end

Widgets.Scrollbar.handle_event = function(self, args)
	if args.type == "mousemotion" then
		if Program.mouse_button_state % 2 == 1 then
			self:set_value_at(Vector(args.x, args.y))
			return
		end
	elseif args.type == "mousepress" then
		if args.button ~= 1 then return end
		if self.max <= self.page then return end
		local cursor = Program.cursor_position
		local c = cursor - Vector(self.x, self.y)
		if c.y < 21 then
			self:scroll("up")
		elseif c.y >= self.height - 21 then
			self:scroll("down")
		else
			self:set_value_at(cursor)
			self:grabbed(true)
		end
		return
	elseif args.type == "mouserelease" then
		if args.button ~= 1 then return end
		self:grabbed(false)
		return
	elseif args.type == "mousescroll" then
		if args.rel > 0 then
			self:scroll("up")
		else
			self:scroll("down")
		end
		return
	end
	return true
end

Widgets.Scrollbar.reshaped = function(self)
	self:set_request{
		internal = true,
		width = 21,
		height = 64}
	local w = self.width
	local h = self.height
	-- Draw the background.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,21},
		dest_size = {w,h-42},
		source_image = "widgets2",
		source_position = {700,0},
		source_tiling = {7,86,7,7,86,7}}
	-- Draw the slider.
	local sy,sh
	local alloc = h - 42
	if self.max > 0 then
		sy = alloc * self.scroll_offset / self.max
		sh = math.min(alloc, math.max(20, alloc * self.page / self.max))
	else
		sy = 0
		sh = alloc
	end
	self:canvas_image{
		dest_position = {0,21 + sy},
		dest_size = {w,sh},
		source_image = "widgets2",
		source_position = {100,100},
		source_tiling = {7,86,7,7,86,7}}
	-- Draw the buttons.
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {21,21},
		source_image = "widgets2",
		source_position = (self.scroll_offset > 0) and {600,0} or {600,200},
		source_tiling = {7,86,7,7,86,7}}
	self:canvas_image{
		dest_position = {0,h-22},
		dest_size = {21,21},
		source_image = "widgets2",
		source_position = (self.scroll_offset < self.max - self.page) and {600,100} or {600,300},
		source_tiling = {7,86,7,7,86,7}}
	self:canvas_compile()
end

Widgets.Scrollbar.scroll = function(self, dir)
	if dir == "up" then
		self.scroll_offset = math.max(0, self.scroll_offset - self.step)
		self:reshaped()
		self:changed(self.scroll_offset)
	elseif dir == "down" then
		self.scroll_offset = math.min(self.scroll_offset + self.step, self.max - self.page)
		self.scroll_offset = math.max(0, self.scroll_offset)
		self:reshaped()
		self:changed(self.scroll_offset)
	end
end

Widgets.Scrollbar.set_range = function(self, max, scroll_offset, page)
	if scroll_offset then self.scroll_offset = scroll_offset end
	if page then self.page = page end
	self.max = max
	self.scroll_offset = math.max(0, math.min(self.scroll_offset, self.max - self.page))
	self:reshaped()
end

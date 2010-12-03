Widgets.ColorSelector = Class(Widget)

Widgets.ColorSelector.setter = function(self, key, value)
	if key == "blue" then
		if self.blue ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "green" then
		if self.green ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "red" then
		if self.red ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	else
		Widget.setter(self, key, value)
	end
end

Widgets.ColorSelector.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.red = self.red or 1
	self.green = self.green or 1
	self.blue = self.blue or 1
	self.init = true
	return self
end

Widgets.ColorSelector.get_value_at = function(self, point)
	local p = point - Vector(self.x, self.y)
	local v = p.x / self.width
	if p.y < self.height / 3 then return "red", v end
	if p.y < self.height * 2 / 3 then return "green", v end
	return "blue", v
end

Widgets.ColorSelector.pressed = function(self)
	local c,v = self:get_value_at(Client.cursor_pos)
	self[c] = v
	return c,v
end

Widgets.ColorSelector.reshaped = function(self)
	if not self.init then return end
	self:set_request{width = 128, height = 24}
	local w = self.width
	local h = self.height
	local values = {self.red, self.green, self.blue}
	self:canvas_clear()
	for i=1,3 do
		local v = values[i]
		if v > 0 then
			self:canvas_image{
				dest_clip = {0,0,v*w,h},
				dest_position = {0,(i-1)*h/3},
				dest_size = {w,h/3},
				source_image = "widgets1",
				source_position = {0,115},
				source_tiling = {7,87,7,4,15,4}}
		end
		if v < 1 then
			self:canvas_image{
				dest_clip = {v*w,0,w-v*w,h},
				dest_position = {0,(i-1)*h/3},
				dest_size = {w,h/3},
				source_image = "widgets1",
				source_position = {0,65},
				source_tiling = {7,87,7,4,15,4}}
		end
	end
--[[	self:canvas_image{
		dest_clip = {0,0,w,h},
		dest_position = {0,8},
		dest_size = {w,h/3},
		source_image = "widgets1",
		source_position = {0,115},
		source_tiling = {7,87,7,4,15,4}}--]]
	self:canvas_compile()
end

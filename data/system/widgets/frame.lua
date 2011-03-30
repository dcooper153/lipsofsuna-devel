Widgets.Frame = Class(Widget)

Widgets.Frame.setter = function(self, key, value)
	if key == "style" then
		if self.style ~= value then
			Widget.setter(self, key, value)
			if value == "default" then
				self.margins = {10,7,10,20}
			elseif value == "equipment" then
				self.spacings = {0,0}
				self.margins = {10,7,0,0}
			elseif value == "list" then
				self.spacings = {0,0}
				self.margins = {0,0,0,0}
			elseif value == "paper" then
				self.margins = {30,30,40,30}
			elseif value == "popup" then
				self.margins = {10,17,5,17}
				self.spacings = {0,0}
			elseif value == "quickbar" then
				self.spacings = {0,16}
				self.margins = {77,0,10,0}
			elseif value == "title" then
				self.margins = {100,100,0,10}
			elseif value == "tooltip" then
				self.margins = {5,16,0,11}
			end
			self:reshaped()
		end
	elseif key == "focused" then
		if self.focused ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "font" then
		if self.font ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "text" then
		if self.text ~= value then
			if self.label then self.label.text = value end
			Widget.setter(self, key, value)
			self:reshaped()
		end
	else
		Widget.setter(self, key, value)
	end
end

--- Creates a new frame widget.
-- @param clss Frame widget class.
-- @param args Arguments.
-- @return Skills widget.
Widgets.Frame.new = function(clss, args)
	local self = Widget.new(clss, {spacings = {5,5}})
	self.style = "default"
	if args then
		for k,v in pairs(args) do self[k] = v end
		if args.text then
			self.cols = 1
			self.rows = 1
			self.label = Widgets.Label{font = "medium", text = args.text, valign = 0.5, halign = 0.5}
			self:append_row(self.label)
			self:set_expand{row = 1, col = 1}
		end
	end
	return self
end

Widgets.Frame.reshaped = function(self)
	local w = self.width
	local h = self.height
	self:canvas_clear()
	if self.style == "default" then
		self:set_request{internal = true, width = 333, height = 150}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {0,0},
			source_tiling = {75,183,75,50,158,100}}
	elseif self.style == "equipment" then
		self:set_request{internal = true, width = 90, height = 143}
		self:canvas_image{
			dest_position = {20,0},
			dest_size = {90,165},
			source_image = "widgets1",
			source_position = {0,327},
			source_tiling = {0,300,0,0,185,0}}
	elseif self.style == "list" then
		self:set_request{internal = true, width = 64, height = 102}
	elseif self.style == "paper" then
		self:set_request{internal = true, width = 333, height = 220}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {0,0},
			source_tiling = {75,183,75,50,158,100}}
		self:canvas_image{
			dest_position = {10,5},
			dest_size = {w-16,h-17},
			source_image = "widgets1",
			source_position = {350,0},
			source_tiling = {50,217,50,100,163,100}}
	elseif self.style == "popup" then
		self:set_request{internal = true, width = 60, height = 40}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {350,455},
			source_tiling = {14,60,24,13,10,27}}
	elseif self.style == "quickbar" then
		self:set_request{internal = true, width = 200, height = 82}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {640,430},
			source_tiling = {256,70,58,0,82,0}}
	elseif self.style == "title" then
		self:set_request{internal = true, width = 333, height = 45}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {672,325},
			source_tiling = {100,73,100,0,45,0}}
	elseif self.style == "tooltip" then
		self:set_request{internal = true, width = 100, height = 20}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {720,0},
			source_tiling = {10,80,21,10,30,21}}
	end
	self:canvas_compile()
end

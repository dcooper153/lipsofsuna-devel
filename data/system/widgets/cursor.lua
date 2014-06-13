--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.widgets.cursor
-- @alias Cursor

local Class = require("system/class")
local Input = require("system/input")
local Widget = require("system/widget")
local Widgets = require("system/widgets")

--- TODO:doc
-- @type Cursor
local Cursor = Class("Cursor", Widget)

Cursor.new = function(clss, cursor)
	local self = Widget.new(clss)
	self.cursor = cursor
	self:set_depth(10)
	return self
end

Cursor.update = function(self)
	-- Update position.
	local c = Input:get_pointer_position()
	self:set_x(c.x + 1)
	self:set_y(c.y + 1)
	-- Show tooltips.
	if self.tooltip then
		self.tooltip:set_floating(false)
		self.tooltip = nil
	end
	if self.widget then return end
	if self:get_floating() then
		local w = Widgets:find_handler_widget("tooltip")
		if w and w.tooltip then
			self.tooltip = w.tooltip
			w.tooltip:popup(c + Vector(5,5))
		end
	end
end

Cursor.reshaped = function(self)
	local icon = self.icon
	local cursor = self.cursor
	self:set_request(
		cursor and cursor.size[1] or 16,
		cursor and cursor.size[2] or 16, true)
	self:canvas_clear()
	if self.cursor then
		self:canvas_image{
			dest_position = {0,0},
			dest_size = cursor.size,
			source_image = cursor.image,
			source_position = cursor.offset,
			source_tiling = {-1,cursor.size[1],-1,-1,cursor.size[2],-1}}
	end
end

return Cursor



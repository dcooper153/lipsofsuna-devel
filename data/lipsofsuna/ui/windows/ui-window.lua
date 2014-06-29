local Class = require("system/class")
local Vector = require("system/math/vector")
local Widget = require("system/widget")

local UiWindow = Class("UiWindow", Widget)

local __vector1 = Vector()

--- Creates a new window.
-- @param class UiWindow class.
-- @return UiWindow.
UiWindow.new = function(clss)
	local self = Widget.new(clss)
	self.__x = 0
	self.__y = 0
	self.__w = 0
	self.__h = 0
	return self
end

--- Handles an input event.
-- @param self Ui class.
-- @param args Event arguments.
-- @return True if the caller should handle the event.
UiWindow.handle_event = function(self, args)
	return true
end

--- Called when the screen size changes.
-- @param self UiWindow.
-- @param width Screen width.
-- @param height Screen height.
UiWindow.screen_resized = function(self, width, height)
end

--- Called once per frame to update the window.
-- @param self UiWindow.
-- @param secs Seconds since the last update.
UiWindow.update = function(self, secs)
end

--- Gets the position of the window.
-- @param self UiWindow.
-- @return X and Y coordinates.
UiWindow.get_position = function(self)
	return self.__x,self.__y
end

--- Sets the position of the window.
-- @param self UiWindow.
-- @param x X coordinate.
-- @param y Y coordinate.
UiWindow.set_position = function(self, x, y)
	self.__x = x
	self.__y = y
	self:set_offset(__vector1:set_xyz(x, y))
end

--- Gets the size of the window.
-- @param self UiWindow.
-- @return Width and height.
UiWindow.get_size = function(self)
	return self.__w,self.__h
end

--- Sets the size of the window.
-- @param self UiWindow.
-- @param w Width.
-- @param h Height.
UiWindow.set_size = function(self, w, h)
	self.__w = w
	self.__h = h
	self:set_request(w, h)
end

--- Finds a widget by screen space position.
-- @param self UiWindow.
-- @param x X coordinate.
-- @param y Y coordinate.
-- @param filter Optional filter function.
-- @return Widget if found. Nil otherwise.
UiWindow.get_widget_by_point = function(self, x, y, filter)
	-- Check for visibility.
	if not self:get_visible() then return end
	-- Check for X coordinate.
	local sx,w = self.__x,self.__w
	if x < sx or sx + w <= point.x then return end
	-- Check for Y coordinate.
	local sy,h = self.__y,self.__h
	if y < sy or sy + h <= point.y then return end
	-- Apply filtering.
	if not filter or filter(self) then return self end
end

return UiWindow

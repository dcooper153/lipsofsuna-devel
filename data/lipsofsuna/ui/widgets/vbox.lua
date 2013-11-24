--- Vertical widget packer.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.widgets.vbox
-- @alias UiVBox

local Class = require("system/class")
local UiWidget = require("ui/widgets/widget")
local Vector = require("system/math/vector")

--- Vertical widget packer.
-- @type UiVBox
local UiVBox = Class("UiVBox", UiWidget)

local __vec1 = Vector()
local __vec2 = Vector()

--- Creates a new vertical widget packer.
-- @param clss UiVBox class.
-- @return UiVBox.
UiVBox.new = function(clss)
	local self = UiWidget.new(clss)
	self.__widgets = {}
	return self
end

--- Appends a child widget.
-- @param self UiVBox.
-- @param widget Widget.
UiVBox.add_child = function(self, widget)
	UiWidget.add_child(self, widget)
	table.insert(self.__widgets, widget)
	self.__repack_needed = true
end

--- Removes all the child widgets.
-- @param self UiVBox.
UiVBox.clear = function(self)
	-- Detach the widgets.
	for k,v in pairs(self.__widgets) do
		v:detach()
	end
	-- Clear the widget list.
	self.__widgets = {}
end

--- Moves the focus down one step.
-- @param self UiVBox.
-- @return True if moved. False otherwise.
UiVBox.focus_down = function(self)
	if not self.focused_item then self.focused_item = 1 end
	if self.focused_item < #self.__widgets then
		local old = self.__widgets[self.focused_item]
		if old then old:set_focused(false) end
		self.focused_item = self.focused_item + 1
		local new = self.__widgets[self.focused_item]
		if new then new:set_focused(true) end
		Client.effects:play_global("uimove1")
		return true
	elseif #self.__widgets > 1 then
		local old = self.__widgets[self.focused_item]
		if old then old:set_focused(false) end
		self.focused_item = 1
		local new = self.__widgets[self.focused_item]
		if new then new:set_focused(true) end
		Client.effects:play_global("uimove1")
		return true
	end
end

--- Focuses the widget at the given index.<br/>
--
-- If the index is too great, it is clamped to the last valid index. If the
-- box is empty or the index is invalid or nil, no widget will be focused.
--
-- @param self UiVBox.
-- @param index Number.
UiVBox.focus_index = function(self, index)
	local f = math.min(index or -1, #self.__widgets)
	if self.__widgets[f] then
		self.focused_item = f
		self.__widgets[f]:set_focused(true)
	else
		self.focused_item = nil
	end
end

--- Moves the focus up one step.
-- @param self UiVBox.
-- @return True if moved. False otherwise.
UiVBox.focus_up = function(self)
	if not self.focused_item then self.focused_item = 1 end
	if self.focused_item > 1 then
		local old = self.__widgets[self.focused_item]
		if old then old:set_focused(false) end
		self.focused_item = self.focused_item - 1
		local new = self.__widgets[self.focused_item]
		if new then new:set_focused(true) end
		Client.effects:play_global("uimove1")
		return true
	elseif #self.__widgets > 1 then
		local old = self.__widgets[self.focused_item]
		if old then old:set_focused(false) end
		self.focused_item = #self.__widgets
		local new = self.__widgets[self.focused_item]
		if new then new:set_focused(true) end
		Client.effects:play_global("uimove1")
		return true
	end
end

--- Focuses the given widget.
-- @param self UiVBox.
-- @param widget Widget.
-- @return Two booleans indicating whether the widget was found and the focus changed, respectively.
UiVBox.focus_widget = function(self, widget)
	for k,v in pairs(self.__widgets) do
		if v == widget then
			-- Check if the focus changed.
			if self.focused_item == k then
				return true,false
			end
			-- Try to unfocus the old widget.
			if self.focused_item then
				local old = self.__widgets[self.focused_item]
				if old and not old:set_focused(false) then return end
			end
			-- Focus the new widget.
			v:set_focused(true)
			self.focused_item = k
			return true,true
		end
	end
end

--- Handles an input event.
-- @param self UiVBox.
-- @param args Event arguments.
-- @return True if the bubbled. False otherwise.
UiVBox.handle_event = function(self, args)
	return true
end

--- Checks if the box is empty.
-- @param self UiVBox.
-- @return True if empty. False otherwise.
UiVBox.is_empty = function(self)
	return not self.__widgets[1]
end

--- Requests the box to be repainted in the next update.
-- @param self UiVBox.
-- @return True if empty. False otherwise.
UiVBox.queue_repaint = function(self)
	for k,v in pairs(self.__widgets) do
		v.need_repaint = true
	end
end

--- Removes a child widget.
-- @param self UiVBox.
-- @param index Number.
-- @return True if removed. False otherwise.
UiVBox.remove_child_by_index = function(self, index)
	local widget = self.__widgets[index]
	if not widget then return end
	widget:detach()
	table.remove(self.__widgets, index)
	return true
end

--- Updates the box.
-- @param self UiVBox.
-- @param secs Seconds since the last update.
UiVBox.update = function(self, secs)
	-- Call the update functions of the widgets.
	for k,v in pairs(self.__widgets) do
		if v.update then v:update(secs) end
	end
	-- Check if repacking is needed.
	if not self.__repack_needed then return end
	self.__repack_needed = nil
	-- Get the offset of the box.
	local x = self.__offset.x
	local y = -self.__offset.y
	local y0 = y
	local sh = Ui.size.y
	-- Repack the child widgets.
	for _,widget in ipairs(self.__widgets) do
		local wh = widget.size.y
		if widget.temporary then
			local wx = widget.temporary.x + x
			local wy = widget.temporary.y - y0
			widget:set_offset(__vec1:set_xyz(wx, wy))
			widget:set_visible(true)--wy > -wh and wy < sh)
		else
			widget:set_offset(__vec1:set_xyz(x, y))
			widget:set_visible(true)--y > -wh and y < sh)
			y = y + wh + Theme.spacing
		end
	end
end

--- Gets an iterator over the children of the widget.
-- @param self UiVBox.
-- @return Iterator.
UiVBox.get_children = function(self)
	return coroutine.wrap(function()
		for k,v in ipairs(self.__widgets) do
			coroutine.yield(k, v)
		end
	end)
end

--- Gets the currently focused widget.
-- @param self UiVBox.
-- @return Widget is found. Nil otherwise.
UiVBox.get_focused_widget = function(self)
	if not self.focused_item then return end
	return self.__widgets[self.focused_item]
end

--- Gets the height of the box in pixels.
-- @param self UiVBox.
-- @return Height.
UiVBox.get_height = function(self)
	local h = 0
	for k,widget in ipairs(self.__widgets) do
		if widget.temporary then
			h = math.max(h, widget.temporary.y + widget.size.y)
		else
			h = h + widget.size.y
			if self.__widgets[k + 1] then
				h = h + Theme.spacing
			end
		end
	end
	return h
end

--- Sets the screen offset of the box.
-- @param self UiVBox.
-- @param value Vector. Units are in pixels.
UiVBox.set_offset = function(self, value)
	self.__offset = value
	self.__repack_needed = true
end

--- Finds a widget by ID.
-- @param self UiVBox.
-- @param id Widget ID.
-- @return Widget if found. Nil otherwise.
UiVBox.get_widget_by_id = function(self, id)
	if not id then return end
	for _,widget in pairs(self.__widgets) do
		if widget.id == id then
			return widget
		end
	end
end

--- Finds a widget by index.
-- @param self UiVBox.
-- @param index Widget index.
-- @return Widget if found. Nil otherwise.
UiVBox.get_widget_by_index = function(self, index)
	return self.__widgets[index]
end

--- Finds the widget that encloses the point.<br/>
--
-- Returns the first widget whose rectangle encloses the given point.
-- If the filter funtion is given, the first matching widget for which the
-- filter returns true will be returned. Other widgets are skipped.
--
-- @param self UiVBox.
-- @param point Point in screen space.
-- @param filter Optional filter function.
-- @return Widget if found. Nil otherwise.
UiVBox.get_widget_by_point = function(self, point, filter)
	for k,v in pairs(self.__widgets) do
		local w = v:get_widget_by_point(point, filter)
		if w then return w end
	end
end

return UiVBox

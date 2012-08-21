local Class = require("system/class")
local Widgets = require("system/widgets")

if not Los.program_load_extension("widgets") then
	error("loading extension `widgets' failed")
end

------------------------------------------------------------------------------

local Widget = Class("Widget")

--- Creates a new widget.
-- @param clss Widget class.
-- @return New widget.
Widget.new = function(clss)
	local self = Class.new(clss)
	self.handle = Los.widget_new()
	self.__children = {}
	__userdata_lookup[self.handle] = self
	return self
end

--- Adds a child widget.
-- @param self Widget.
-- @param widget Widget.
Widget.add_child = function(self, widget)
	Los.widget_add_child(self.handle, widget.handle)
	self.__children[widget] = widget
end

--- Calculates and sets the size request of the widget.
-- @param self Widget.
-- @param args Arguments.<ul>
--   <li>1,width: Width request.</li>
--   <li>2,height: Height request.</li>
--   <li>font: Font to use when calculating from text.</li>
--   <li>internal: Set the internal request instead of the user request.</li>
--   <li>paddings: Additional paddings to add or nil.</li>
--   <li>text: Text to use when calculating from text.</li></ul>
Widget.calculate_request = function(self, ...)
	Los.widget_set_request(self.handle, ...)
end

--- Clears the canvas of the widget.
-- @param self Widget.
Widget.canvas_clear = function(self)
	Los.widget_canvas_clear(self.handle)
end

--- Compiles the canvas of the widget.
-- @param self Widget.
Widget.canvas_compile = function(self)
	Los.widget_canvas_compile(self.handle)
end

--- Packs an image to the canvas of the widget.
-- @param self Widget.
-- @param args Arguments.<ul>
--   <li>color: RGBA color or nil.</li>
--   <li>dest_clip: {x,y,w,h} or nil.</li>
--   <li>dest_position: {x,y} or nil.</li>
--   <li>dest_size: {w,h} or nil.</li>
--   <li>rotation: Rotation angle in radians or nil.</li>
--   <li>rotation_center: Rotation center vector or nil.</li>
--   <li>source_position: {x,y} or nil.</li>
--   <li>source_tiling: {x1,x2,x3,y1,y2,y3} or nil.</li></ul>
Widget.canvas_image = function(self, args)
	local a = {}
	for k,v in pairs(args) do a[k] = v end
	if a.rotation_center then a.rotation_center = a.rotation_center.handle end
	Los.widget_canvas_image(self.handle, a)
end

--- Packs text to the canvas of the widget.
-- @param self Widget.
-- @param args Arguments.<ul>
--   <li>dest_clip: {x,y,w,h} or nil.</li>
--   <li>dest_position: {x,y} or nil.</li>
--   <li>dest_size: {w,h} or nil.</li>
--   <li>rotation: Rotation angle in radians or nil.</li>
--   <li>rotation_center: Rotation center vector or nil.</li>
--   <li>text: String.</li>
--   <li>text_alignment: {x,y} or nil.</li>
--   <li>text_color: {r,g,b,a} or nil.</li>
--   <li>text_font: Font name or nil.</li></ul>
Widget.canvas_text = function(self, args)
	local a = {}
	for k,v in pairs(args) do a[k] = v end
	if a.rotation_center then a.rotation_center = a.rotation_center.handle end
	Los.widget_canvas_text(self.handle, a)
end

--- Removes the widget from its parent.
-- @param self Widget.
Widget.detach = function(self)
	-- Remove from the script parent.
	local p = self:get_parent()
	if p then
		for k,v in pairs(p.__children) do
			if v == self then
				p.__children[k] = nil
				break
			end
		end
	end
	-- Remove from the internal parent.
	Los.widget_detach(self.handle)
end

--- Pops up the widget.
-- @param self Widget.
-- @param args Arguments.<ul>
--   <li>x: X coordinate.</li>
--   <li>y: Y coordinate.</li>
--   <li>width: Width allocation.</li>
--   <li>height: Height allocation.</li>
--   <li>dir: Popup direction. ("left"/"right"/"up"/"down")</li></ul>
Widget.popup = function(self, args)
	if Widgets.popup then
		Widgets.popup:set_visible(false)
	end
	Widgets.popup = self
	return Los.widget_popup(self.handle, args)
end

--- Gets the focus status of the widget.
-- @param self Widget.
-- @param v Boolean.
Widget.set_focused = function(self, v)
	self.focused = v
end

--- Gets the depth sorting priority of the widget.
-- @param self Widget.
-- @return Number.
Widget.get_depth = function(self)
	return Los.widget_get_depth(self.handle)
end

--- Sets the depth sorting priority of the widget.
-- @param self Widget.
-- @param v Number.
Widget.set_depth = function(self, v)
	Los.widget_set_depth(self.handle, v)
end

--- Gets the floating flag of the widget.
-- @param self Widget.
-- @return Boolean.
Widget.get_floating = function(self)
	return Los.widget_get_floating(self.handle)
end

--- Sets the floating flag of the widget.
-- @param self Widget.
-- @param v Boolean.
Widget.set_floating = function(self, v)
	Los.widget_set_floating(self.handle, v)
	if not v and self == Widgets.popup then
		Widgets.popup = nil
	end
end

--- Gets the fullscreen enable flag of the widget.
-- @param self Widget.
-- @return Boolean.
Widget.get_fullscreen = function(self)
	return Los.widget_get_fullscreen(self.handle)
end

--- Sets the fullscreen enable flag of the widget.
-- @param self Widget.
-- @param v Boolean.
Widget.set_fullscreen = function(self, v)
	Los.widget_set_fullscreen(self.handle, v)
end

--- Gets the height of the widget.
-- @param self Widget.
-- @return Number.
Widget.get_height = function(self)
	return Los.widget_get_height(self.handle)
end

--- Gets the pixel offset of the widget relative to its parent.
-- @param self Widget.
-- @return Vector.
Widget.get_offset = function(self)
	return Vector:new_from_handle(Los.widget_get_offset(self.handle))
end

--- Sets the pixel offset of the widget relative to its parent.
-- @param self Widget.
-- @param v Vector.
Widget.set_offset = function(self, v)
	Los.widget_set_offset(self.handle, v.handle)
end

--- The parent of this widget.
-- @name Widget.parent
-- @class table
Widget.get_parent = function(self)
	local handle = Los.widget_get_parent(self.handle)
	if not handle then return end
	return __userdata_lookup[handle] -- TODO: eliminate by caching the parent in scripts
end

--- Gets the size request of the widget.
-- @param self Widget.
-- @return Width, height.
Widget.get_request = function(self)
	return Los.widget_get_request(self.handle)
end

--- Sets the size request of the widget.
-- @param self Widget.
-- @param width Width request, or nil.
-- @param height Height request, or nil.
-- @param internal True to set the internal request instead of the user request.
Widget.set_request = function(self, width, height, internal)
	Los.widget_set_request(self.handle, {width = width, height = height, internal = internal})
end

--- Gets the visibility flag of the widget.
-- @param self Widget.
-- @return Boolean.
Widget.get_visible = function(self)
	return Los.widget_get_visible(self.handle)
end

--- Sets the visibility flag of the widget.
-- @param self Widget.
-- @param v Boolean.
Widget.set_visible = function(self, v)
	Los.widget_set_visible(self.handle, v)
	if not v and self == Widgets.popup then
		Widgets.popup = nil
	end
end

--- Gets the width of the widget
-- @param self Widget.
-- @return Number.
Widget.get_width = function(self)
	return Los.widget_get_width(self.handle)
end

--- Gets the left edge position.
-- @param self Widget.
-- @return Number.
Widget.get_x = function(self)
	return Los.widget_get_x(self.handle)
end

--- Sets the left edge position.
-- @param self Widget.
-- @return Number.
Widget.set_x = function(self, v)
	Los.widget_set_x(self.handle, v)
end

--- Gets the top edge position of the widget.
-- @param self Widget.
-- @return Number.
Widget.get_y = function(self)
	return Los.widget_get_y(self.handle)
end

--- Sets the top edge position of the widget.
-- @param self Widget.
-- @param v Number.
Widget.set_y = function(self, v)
	Los.widget_set_y(self.handle, v)
end

-- Userdata don't have members but the engine needs to be able to call the
-- reshape functions of widgets. This is done with a global variable.
__widget_reshape = function(handle)
	local w = __userdata_lookup[handle]
	if w and w.reshaped then w:reshaped() end
end

return Widget

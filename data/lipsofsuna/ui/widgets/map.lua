local Class = require("system/class")
local UiIcon = require("ui/widgets/icon")
local UiTooltip = require("ui/widgets/tooltip")
local UiWidget = require("ui/widgets/widget")

local UiMap = Class("Uimap", UiWidget)

--- Creates a new map widget.
-- @return Map widget.
UiMap.new = function(clss)
	local self = UiWidget.new(clss)
	self.hint = "$$B\n$U: Zoom in\n$D: Zoom out\n"
	self.markers = {}
	self.timer = 0
	return self
end

--- Adds a marker to the display.
-- @param self Map widget.
-- @param icon Icon name of the marker.
-- @param name Marker name.
-- @param pos World space position of the marker.
-- @param rot World space rotation of the marker.
UiMap.add_marker = function(self, icon, name, pos, rot)
	-- Calculate the relative position.
	local center = Client.player_object:get_position()
	local loc = Vector(pos.x, pos.z) - Vector(center.x, center.z)
	-- Calculate the pixel position.
	local size = self.size
	local scale = Client.data.map.scale
	local pix = loc * scale + size * 0.5 - Vector(icon.size) * 0.5
	if pix.x < 0 or pix.y < 0 or pix.x >= size.x or pix.y >= size.y then return end
	-- Create the widget.
	local tooltip = string.format("%s\nDistance: %d\nDepth: %d", name, loc.length, pos.y - center.y)
	local widget = UiIcon(Main.specs:find_by_name("IconSpec", icon), pix, rot, UiTooltip(tooltip))
	-- Pack the widget.
	self:add_child(widget)
	table.insert(self.markers, widget)
end

--- Clear the displayed marker.
-- @param self Map widget.
UiMap.clear_markers = function(self)
	if self.markers then
		for k,v in pairs(self.markers) do v:detach() end
	end
	self.markers = {}
end

UiMap.handle_event = function(self, args)
	-- Zoom with keyboard.
	if args.type ~= "keyrelease" then
		local a = {}
		for k,v in pairs(args) do a[k] = v end
		if args.type == "keyrepeat" then
			a.type = "keypress"
		end
		local action1 = Client.bindings:find_by_name("menu_up")
		local action2 = Client.bindings:find_by_name("menu_down")
		if (action1 and action1:get_event_response(a) ~= nil) then
			self:zoom("in")
			return
		elseif (action2 and action2:get_event_response(a) ~= nil) then
			self:zoom("out")
			return
		end
	end
	-- Zoom with mouse.
	if not Ui:get_pointer_grab() and args.type == "mousescroll" then
		if args.rel > 0 then
			self:zoom("in")
		else
			self:zoom("out")
		end
		return
	end
	-- Other actions.
	return UiWidget.handle_event(self, args)
end

UiMap.rebuild_size = function(self)
	return Vector(Theme.text_height_1 * 10, Theme.text_height_1 * 10)
end

UiMap.rebuild_canvas = function(self)
	-- Add the base.
	UiWidget.rebuild_canvas(self)
	-- Rebuild the markers.
	local player = Client.player_object
	self:clear_markers()
	self:add_marker("mapmarker2", "player", player:get_position(), player:get_rotation().euler[1])
	for k,v in pairs(Main.markers.__dict_name) do
		if v.unlocked then
			self:add_marker("mapmarker1", k, v.position, 0)
		end
	end
end

UiMap.update = function(self, secs)
	self.timer = self.timer + secs
	if self.timer > 0.1 then
		self.timer = 0
		self.need_repaint = true
	end
	UiWidget.update(self, secs)
end

UiMap.zoom = function(self, dir)
	if dir == "in" then
		Client.data.map.scale = Client.data.map.scale * 1.5
	else
		Client.data.map.scale = Client.data.map.scale / 1.5
	end
	self.need_repaint = true
end

return UiMap

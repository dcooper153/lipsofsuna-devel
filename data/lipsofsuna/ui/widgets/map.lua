require(Mod.path .. "widget")

Widgets.Uimap = Class(Widgets.Uiwidget)
Widgets.Uimap.class_name = "Widgets.Uimap"

--- Creates a new map widget.
-- @return Map widget.
Widgets.Uimap.new = function(clss)
	local self = Widgets.Uiwidget.new(clss)
	self.hint = "$$B\n$$U\n$$D"
	self.markers = {}
	return self
end

--- Adds a marker to the display.
-- @param self Map widget.
-- @param icon Icon name of the marker.
-- @param name Marker name.
-- @param pos World space position of the marker.
-- @param rot World space rotation of the marker.
Widgets.Uimap.add_marker = function(self, icon, name, pos, rot)
	-- Calculate the relative position.
	local center = Client.player_object.position
	local loc = Vector(pos.x, pos.z) - Vector(center.x, center.z)
	-- Calculate the pixel position.
	local size = self.size
	local scale = Voxel.tile_scale * Client.data.map.scale
	local pix = loc * scale + size * 0.5 - Vector(icon.size) * 0.5
	if pix.x < 0 or pix.y < 0 or pix.x >= size.x or pix.y >= size.y then return end
	-- Create the widget.
	local tooltip = string.format("%s\nDistance: %d\nDepth: %d", name, loc.length, pos.y - center.y)
	local widget = Widgets.Icon{
		icon = Iconspec:find{name = icon}, offset = pix,
		rotation = rot, tooltip = Widgets.Tooltip{text = tooltip}}
	-- Pack the widget.
	self:add_child(widget)
	table.insert(self.markers, widget)
end

--- Clear the displayed marker.
-- @param self Map widget.
Widgets.Uimap.clear_markers = function(self)
	if self.markers then
		for k,v in pairs(self.markers) do v:detach() end
	end
	self.markers = {}
end

Widgets.Uimap.handle_event = function(self, args)
	if not Program.cursor_grabbed then
		if args.type == "mousescroll" then
			if args.rel > 0 then
				Client.data.map.scale = Client.data.map.scale * 1.5
			else
				Client.data.map.scale = Client.data.map.scale / 1.5
			end
			self.need_repaint = true
			return
		end
	end
	return Widgets.Uiwidget.handle_event(self, args)
end

Widgets.Uimap.rebuild_size = function(self)
	local size = Widgets.Uiwidget.rebuild_size(self)
	size.y = math.max(size.y, 250)
	return size
end

Widgets.Uimap.rebuild_canvas = function(self)
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Rebuild the markers.
	local player = Client.player_object
	self:clear_markers()
	self:add_marker("mapmarker2", "player", player.position, player.rotation.euler[1])
	for k,v in pairs(Marker.dict_name) do
		self:add_marker("mapmarker1", k, v.position, 0)
	end
end

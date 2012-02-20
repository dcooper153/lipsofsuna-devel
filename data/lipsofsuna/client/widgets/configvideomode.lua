
Widgets.ConfigVideomode = Class(Widget)
Widgets.ConfigVideomode.class_name = "Widgets.ConfigVideomode"

Widgets.ConfigVideomode.new = function(clss)
	-- Allocate self.
	local self = Widget.new(clss)
	self:set_request{width = 300, height = 20}
	-- Create a sorted list of video modes.
	-- Some of the views don't fit in the screen unless the height is at least
	-- 720 pixels so we filter out any modes smaller than that.
	self.modes = {}
	for k,v in ipairs(Program.video_modes) do
		if v[2] >= 720 then
			table.insert(self.modes, {"" .. v[1] .. "x" .. v[2], function()
				Program:set_video_mode(v[1], v[2], true, Client.options.vsync)
			end, width = v[1], height = v[2]})
		end
	end
	table.sort(self.modes, function(a, b)
		if a.height < b.height then return true end
		if a.height > b.height then return false end
		if a.width < b.width then return true end
		return false
	end)
	table.insert(self.modes, 1, {"Windowed", function()
		local s = Program.video_mode
		Program:set_video_mode(s[1], s[2], false, Client.options.vsync)
	end})
	-- Create the label widget.
	self.label = Widgets.Label{text = "Resolution", valign = 0.5}
	self.label:set_request{width = 150, height = 20}
	self:add_child(self.label)
	-- Create the child widget.
	self.child = Widgets.ComboBox(self.modes)
	self.child.offset = Vector(150, 0)
	self.child:set_request{width = 150, height = 20}
	self:add_child(self.child)
	-- Set the initial selection.
	self:update_selection()
	return self
end

Widgets.ConfigVideomode.update_selection = function(self)
	if Client.options.fullscreen then
		for k,v in ipairs(self.modes) do
			if v.width == Client.options.window_width and
			   v.height == Client.options.window_height then
				return self.child:activate{index = k, press = false}
			end
		end
	end
	self.child:activate{index = 1, press = false}
end

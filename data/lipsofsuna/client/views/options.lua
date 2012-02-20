require "client/widgets/configoption"

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

Views.Options = Class(Widget)
Views.Options.class_name = "Views.Options"

Views.Options.new = function(clss)
	-- Allocate self.
	local self = Widget.new(clss, {cols = 2, rows = 4, spacings = {5,0}})
	self.config_videomode = Widgets.ConfigVideomode()
	-- Packing.
	local quality_group = Widgets.Frame{cols = 2}
	quality_group:append_row(self.config_videomode)
	quality_group:append_row(Widgets.ConfigOption("multisamples"))
	quality_group:append_row(Widgets.ConfigOption("anisotropic_filter", function(k,v) Render.anisotrophy = v end))
	quality_group:append_row(Widgets.ConfigOption("vsync"))
	quality_group:append_row(Widgets.ConfigOption("outlines_enabled", function(k,v) self.outlines_enabled = v end))
	quality_group:append_row(Widgets.ConfigOption("shadow_casting_actors"))
	quality_group:append_row(Widgets.ConfigOption("shadow_casting_items"))
	quality_group:append_row(Widgets.ConfigOption("shadow_casting_obstacles"))
	quality_group:append_row(Widgets.ConfigOption("bloom_enabled", function(k,v) self.bloom_enabled = v end))
	local mouse_group = Widgets.Frame{cols = 1}
	mouse_group:append_row(Widgets.ConfigOption("mouse_sensitivity"))
	mouse_group:append_row(Widgets.ConfigOption("mouse_smoothing", function(k,v) Client.mouse_smoothing = v end))
	mouse_group:append_row(Widgets.ConfigOption("invert_mouse"))
	mouse_group:append_row(Widgets.ConfigOption("music_volume", function(k,v) Sound.music_volume = v end))
	mouse_group:append_row(Widgets.ConfigOption("sound_volume"))
	mouse_group:append_row(Widgets.ConfigOption("nudity_enabled"))
	self.title_graphics = Widgets.Title{text = "Graphics",
		back = function() self:back() end,
		close = function() self:exit() end,
		help = function() Client.views.help:show("options") end}
	self:set_child(1, 1, self.title_graphics)
	self:set_child(1, 2, quality_group)
	self:set_child(2, 1, Widgets.Frame{style = "title", text = "Gameplay"})
	self:set_child(2, 2, mouse_group)
	-- Load the options.
	self:load()
	-- Prepare the standalone mode.
	self.button_back = Widgets.Label{font = "mainmenu", text = "Back", pressed = function() self:back() end}
	self.group_buttons = Widget{cols = 3, rows = 1, margins = {bottom = 30}, spacings = {horz = 30}}
	self.group_buttons:set_expand{col = 1}
	self.group_buttons:set_expand{col = 3}
	self.group_buttons:set_child(2, 1, self.button_back)
	self.background = Widgets.Background{cols = 3, rows = 3, fullscreen = true, image = "mainmenu1"}
	self.background:set_expand{col = 1, row = 1}
	self.background:set_expand{col = 3}
	self.background:set_child(2, 3, self.group_buttons)
	return self
end

Views.Options.back = function(self)
	if self.background.floating then
		Client:set_mode("login")
	else
		Client:set_mode("menu")
	end
end

--- Closes the options view.
-- @param self Options view.
Views.Options.close = function(self)
	-- Close standalone mode.
	if self.background.floating then
		self.background.floating = false
		self.background:set_child(2, 2, nil)
	end
end

Views.Options.exit = function(self)
	if self.background.floating then
		Client:set_mode("login")
	else
		Client:set_mode("game")
	end
end

--- Shows the options screen.
-- @param self Options.
-- @param from Name of the previous mode.
Views.Options.enter = function(self, from, level)
	if from == "login" then
		-- Standalone mode if opened from the login screen.
		self.background:set_child(2, 2, self)
		self.background.floating = true
	else
		-- Menu mode if opened when playing.
		Gui.menus:open{level = level, widget = self}
	end
end

Views.Options.load = function(self)
	-- Update the selection of the video mode widget.
	self.config_videomode:update_selection()
end

Views.Options:add_setters{
	bloom_enabled = function(self, v)
		if v then
			Render:add_compositor("bloom1")
		else
			Render:remove_compositor("bloom1")
		end
	end,
	outlines_enabled = function(self, v)
		Render.material_scheme = v and "Default" or "no-outline"
	end}

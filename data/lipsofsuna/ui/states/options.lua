local Render = require("system/render")
local Sound = require("system/sound")
local UiRadioMenu = require("ui/widgets/radio-menu")

Ui:add_state{
	state = "options",
	label = "Options",
	background = function()
		if Client.player_object then return end
		return Widgets.Uibackground("mainmenu1")
	end}

Ui:add_widget{
	state = "options",
	widget = function()
		return UiRadioMenu("Video mode", function(self)
			-- Get the sorted list of video modes.
			local mode = Program:get_video_mode()
			local modes = {}
			for k,v in ipairs(Program:get_video_modes()) do
				if v[2] >= 480 then
					local name = string.format("%sx%s", v[1], v[2])
					table.insert(modes, {name, v[1], v[2], true})
				end
			end
			table.sort(modes, function(a, b)
				if a[3] < b[3] then return true end
				if a[3] > b[3] then return false end
				if a[2] < b[2] then return true end
				return false
			end)
			-- Create the windowed mode button.
			self:clear()
			self:add_item("Windowed", not mode[3], function(w)
				local s = Program:get_video_mode()
				Program:set_video_mode(s[1], s[2], false, Client.options.vsync)
			end)
			-- Create the fullscreen mode buttons.
			for k,v in ipairs(modes) do
				self:add_item(v[1], mode[3] and v[2] == mode[1] and v[3] == mode[2], function(w)
					Program:set_video_mode(v[2], v[3], v[4], Client.options.vsync)
				end)
			end
		end)
	end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("multisamples") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("anisotropic_filter", function(k,v) Render:set_anisotrophy(v) end) end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("vsync") end}

Ui:add_widget{
	state = "options",
	widget = function()
		return Widgets.Uiconfigoption("landmark_view_distance",
			function() Client.options:apply() end)
		end}

Ui:add_widget{
	state = "options",
	widget = function()
		return Widgets.Uiconfigoption("shader_quality",
			function() Client.options:apply() end)
		end}

Ui:add_widget{
	state = "options",
	widget = function()
		return Widgets.Uiconfigoption("outlines_enabled",
			function() Client.options:apply() end)
		end}

Ui:add_widget{
	state = "options",
	widget = function()
		return Widgets.Uiconfigoption("shadow_casting_actors",
			function() Client.options:apply() end)
	end}

Ui:add_widget{
	state = "options",
	widget = function()
		return Widgets.Uiconfigoption("shadow_casting_items",
			function() Client.options:apply() end)
	end}

Ui:add_widget{
	state = "options",
	widget = function()
		return Widgets.Uiconfigoption("shadow_casting_obstacles",
			function() Client.options:apply() end)
	end}

Ui:add_widget{
	state = "options",
	widget = function()
		return Widgets.Uiconfigoption("bloom_enabled",
			function() Client.options:apply() end)
	end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("mouse_sensitivity") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("ui_animations") end}

Ui:add_widget{
	state = "options",
	widget = function()
		return Widgets.Uiconfigoption("ui_size",
			function() Client.options:apply() end)
	end}

Ui:add_widget{
	state = "options",
	widget = function()
		if not Client.camera_manager then return end
		return Widgets.Uiconfigoption("mouse_smoothing", function(k,v) Client.camera_manager:set_mouse_smoothing(v) end)
	end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("invert_mouse") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("music_volume", function(k,v) Sound:set_music_volume(v) end) end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("sound_volume") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("help_messages") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("nudity_enabled") end}

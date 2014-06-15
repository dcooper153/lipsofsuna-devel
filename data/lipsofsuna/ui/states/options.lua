local Graphics = require("system/graphics")
local Render = require("system/render")
local Sound = require("system/sound")
local UiBackground = require("ui/widgets/background")
local UiConfigOption = require("ui/widgets/config-option")
local UiRadioMenu = require("ui/widgets/radio-menu")
local UiStateMenu = require("ui/widgets/state-menu")

Ui:add_state{
	state = "options",
	label = "Options",
	background = function()
		if Client.player_object then return end
		return UiBackground("mainmenu1")
	end}

Ui:add_widget{
	state = "options",
	widget = function()
		return UiRadioMenu("Video mode", function(self)
			-- Get the sorted list of video modes.
			local mode = Graphics:get_video_mode()
			local modes = {}
			for k,v in ipairs(Graphics:get_video_modes()) do
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
				local s = Graphics:get_video_mode()
				Graphics:set_video_mode(s[1], s[2], false, Client.options.vsync)
			end)
			-- Create the fullscreen mode buttons.
			for k,v in ipairs(modes) do
				self:add_item(v[1], mode[3] and v[2] == mode[1] and v[3] == mode[2], function(w)
					Graphics:set_video_mode(v[2], v[3], v[4], Client.options.vsync)
				end)
			end
		end)
	end}

Ui:add_widget{
	state = "options",
	widget = function() return UiStateMenu("Graphics", "options/graphics") end}

Ui:add_widget{
	state = "options",
	widget = function() return UiStateMenu("Sound", "options/sound") end}

Ui:add_widget{
	state = "options",
	widget = function() return UiStateMenu("Mouse", "options/mouse") end}

Ui:add_widget{
	state = "options",
	widget = function() return UiStateMenu("User interface", "options/ui") end}

------------------------------------------------------------------------------

Ui:add_widget{
	state = "options/graphics",
	widget = function()
		return UiConfigOption("shader_quality",
			function() Client.options:apply() end)
		end}

Ui:add_widget{
	state = "options/graphics",
	widget = function() return UiConfigOption("multisamples") end}

Ui:add_widget{
	state = "options/graphics",
	widget = function() return UiConfigOption("anisotropic_filter", function(k,v) Render:set_anisotrophy(v) end) end}

Ui:add_widget{
	state = "options/graphics",
	widget = function()
		return UiConfigOption("landmark_view_distance",
			function() Client.options:apply() end)
		end}

Ui:add_widget{
	state = "options/graphics",
	widget = function() return UiConfigOption("vsync") end}

Ui:add_widget{
	state = "options/graphics",
	widget = function()
		return UiConfigOption("outlines_enabled",
			function() Client.options:apply() end)
		end}

Ui:add_widget{
	state = "options/graphics",
	widget = function()
		return UiConfigOption("bloom_enabled",
			function() Client.options:apply() end)
	end}

Ui:add_widget{
	state = "options/graphics",
	widget = function()
		return UiConfigOption("shadow_casting_actors",
			function() Client.options:apply() end)
	end}

Ui:add_widget{
	state = "options/graphics",
	widget = function()
		return UiConfigOption("shadow_casting_items",
			function() Client.options:apply() end)
	end}

Ui:add_widget{
	state = "options/graphics",
	widget = function()
		return UiConfigOption("shadow_casting_obstacles",
			function() Client.options:apply() end)
	end}

Ui:add_widget{
	state = "options/graphics",
	widget = function()
		return UiConfigOption("softbody_enabled", function()
			Client.options:apply()
		end)
	end}

Ui:add_widget{
	state = "options/graphics",
	widget = function()
		return UiConfigOption("nudity_enabled", function()
			Client.options:apply()
		end)
	end}

------------------------------------------------------------------------------

Ui:add_widget{
	state = "options/mouse",
	widget = function() return UiConfigOption("mouse_sensitivity") end}

Ui:add_widget{
	state = "options/mouse",
	widget = function() return UiConfigOption("invert_mouse") end}

Ui:add_widget{
	state = "options/mouse",
	widget = function()
		if not Client.camera_manager then return end
		return UiConfigOption("mouse_smoothing", function(k,v) Client.camera_manager:set_mouse_smoothing(v) end)
	end}

------------------------------------------------------------------------------

Ui:add_widget{
	state = "options/ui",
	widget = function() return UiConfigOption("ui_animations") end}

Ui:add_widget{
	state = "options/ui",
	widget = function() return UiConfigOption("help_messages") end}

Ui:add_widget{
	state = "options/ui",
	widget = function()
		return UiConfigOption("ui_size",
			function() Client.options:apply() end)
	end}

------------------------------------------------------------------------------

Ui:add_widget{
	state = "options/sound",
	widget = function() return UiConfigOption("music_volume", function(k,v) Sound:set_music_volume(v) end) end}

Ui:add_widget{
	state = "options/sound",
	widget = function() return UiConfigOption("sound_volume") end}

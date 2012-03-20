Ui:add_state{
	state = "options",
	label = "Options",
	background = function()
		if Client.player_object then return end
		return Widgets.Background{fullscreen = true, image = "mainmenu1"}
	end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uitransition("Video mode", "options/videomode") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("multisamples") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("anisotropic_filter", function(k,v) Render.anisotrophy = v end) end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("vsync") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("outlines_enabled",
		function() Client:update_rendering_style() end) end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("shadow_casting_actors") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("shadow_casting_items") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("shadow_casting_obstacles") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("bloom_enabled",
		function() Client:update_rendering_style() end) end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("mouse_sensitivity") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("mouse_smoothing", function(k,v) Client.mouse_smoothing = v end) end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("invert_mouse") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("music_volume", function(k,v) Sound.music_volume = v end) end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("sound_volume") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("help_messages") end}

Ui:add_widget{
	state = "options",
	widget = function() return Widgets.Uiconfigoption("nudity_enabled") end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "options/videomode",
	label = "Video mode",
	background = function()
		if Client.player_object then return end
		return Widgets.Background{fullscreen = true, image = "mainmenu1"}
	end,
	init = function()
		-- Get the sorted list of video modes.
		modes = {}
		for k,v in ipairs(Program.video_modes) do
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
		widgets = {
			Widgets.Uiradio("Windowed", "mode", function(w)
				local s = Program.video_mode
				Program:set_video_mode(s[1], s[2], false, Client.options.vsync)
			end)}
		-- Create the fullscreen mode buttons.
		for k,v in ipairs(modes) do
			local widget = Widgets.Uiradio(v[1], "mode", function(w)
				Program:set_video_mode(w.mode[2], w.mode[3], w.mode[4], Client.options.vsync)
			end)
			widget.mode = v
			table.insert(widgets, widget)
		end
		-- Activate the button of the current mode.
		local mode = Program.video_mode
		for k,v in pairs(widgets) do
			if not v.mode then
				-- Windowed.
				if not mode[3] then
					v.value = true
					break
				end
			else
				-- Fullscreen.
				if v.mode[2] == mode[1] and v.mode[3] == mode[2] then
					v.value = true
					break
				end
			end
		end
		return widgets
	end}

Options = Class()
Options.fullscreen = false
Options.vsync = false
Options.window_width = 1024
Options.window_height = 768
Options.model_quality = 1
Options.animation_quality = 1.0
Options.mouse_sensitivity = 1.0
Options.multisamples = 2
Options.transparency_quality = 0.3
Options.sound_volume = 1.0
Options.music_volume = 0.1

Options.init = function(clss)
	-- Create a sorted list of video modes.
	clss.video_modes = {}
	for k,v in ipairs(Client.video_modes) do
		table.insert(clss.video_modes, {"" .. v[1] .. "x" .. v[2], function()
			clss:set_video_mode(v[1], v[2], true)
		end, width = v[1], height = v[2]})
	end
	table.sort(clss.video_modes, function(a, b)
		if a.height < b.height then return true end
		if a.height > b.height then return false end
		if a.width < b.width then return true end
		return false
	end)
	table.insert(clss.video_modes, 1, {"Windowed", function()
		local s = Client.video_mode
		clss:set_video_mode(s[1], s[2], false)
	end})
	-- Video mode.
	clss.combo_video_mode = Widgets.ComboBox(clss.video_modes)
	clss.combo_video_mode:activate{index = 1, press = false}
	-- Antialiasing quality.
	local samples = math.log(clss.multisamples) / math.log(2)
	clss.scroll_multisamples = Widgets.Progress{min = 0, max = 4, text = tostring(clss.multisamples) .. "x", value = samples}
	clss.scroll_multisamples:set_request{width = 100}
	clss.scroll_multisamples.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		clss:set_multisamples(2 ^ math.floor(v + 0.5))
	end
	-- Model quality adjustment.
	clss.button_model_quality = Widgets.Button{text = "High quality"}
	clss.button_model_quality.pressed = function(self)
		if self.text == "Low quality" then
			clss:set_model_quality(1)
		else
			clss:set_model_quality(0)
		end
	end
	-- VSync enable.
	clss.button_vsync = Widgets.Button{text = "Disabled"}
	clss.button_vsync.pressed = function(self)
		clss:set_vsync(not clss.vsync)
	end
	-- Animation quality adjustment.
	clss.scroll_animation = Widgets.Progress{min = 0, max = 1, value = clss.animation_quality}
	clss.scroll_animation:set_request{width = 100}
	clss.scroll_animation.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		clss:set_animation_quality(v)
	end
	-- Transparency quality adjustment.
	clss.scroll_transparency = Widgets.Progress{min = 0, max = 1, value = clss.transparency_quality}
	clss.scroll_transparency:set_request{width = 100}
	clss.scroll_transparency.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		clss:set_transparency_quality(v)
	end
	-- Bloom toggle.
	local button_bloom = Widgets.Button{text = "Bloom disabled"}
	button_bloom.pressed = function(self)
		if self.text == "Bloom disabled" then
			clss.bloom_enabled = true
			self.text = "Bloom enabled"
		else
			clss.bloom_enabled = nil
			self.text = "Bloom disabled"
		end
		clss:changed()
	end
	-- Luminance adjustment.
	local scroll_luminance = Widgets.Progress{min = 0, max = 1, value = 0}
	scroll_luminance:set_request{width = 100}
	scroll_luminance.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		self.value = v
		Bloom.luminance = 1 - v
		Bloom:compile()
		clss:changed()
	end
	-- Radius adjustment.
	local scroll_radius = Widgets.Progress{value = 40}
	scroll_radius:set_request{width = 100}
	scroll_radius.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		self.value = v
		Bloom.radius = math.floor(v)
		Bloom:compile()
		clss:changed()
	end
	-- Exposure adjustment.
	local scroll_exposure = Widgets.Progress{min = 1, max = 10, value = 1.5}
	scroll_exposure:set_request{width = 100}
	scroll_exposure.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		self.value = v
		Bloom.exposure = v
		Bloom:compile()
		clss:changed()
	end
	-- Mouse sensitivity.
	clss.scroll_mouse = Widgets.Progress{min = 0, max = 2, value = clss.mouse_sensitivity}
	clss.scroll_mouse:set_request{width = 100}
	clss.scroll_mouse.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		clss:set_mouse_sensitivity(v)
	end
	-- Sound settings.
	clss.scroll_music = Widgets.Progress{min = 0, max = 0.5, value = clss.music_volume}
	clss.scroll_music:set_request{width = 100}
	clss.scroll_music.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos) 
		clss:set_music_volume(v)
	end
	clss.scroll_sound = Widgets.Progress{min = 0, max = 1, value = clss.sound_volume}
	clss.scroll_sound:set_request{width = 100}
	clss.scroll_sound.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		clss:set_sound_volume(v)
	end
	-- Packing.
	local quality_group = Widget{cols = 2}
	quality_group:append_row(Widgets.Label{text = "Resolution"}, clss.combo_video_mode)
	quality_group:append_row(Widgets.Label{text = "Models"}, clss.button_model_quality)
	quality_group:append_row(Widgets.Label{text = "Antialiasing"}, clss.scroll_multisamples)
	quality_group:append_row(Widgets.Label{text = "Animation"}, clss.scroll_animation)
	quality_group:append_row(Widgets.Label{text = "Transparency"}, clss.scroll_transparency)
	quality_group:append_row(Widgets.Label{text = "VSync"}, clss.button_vsync)
	local bloom_group = Widget{cols = 2}
	bloom_group:append_row(Widgets.Label{text = "Bloom"}, button_bloom)
	bloom_group:append_row(Widgets.Label{text = "Radius"}, scroll_radius)
	bloom_group:append_row(Widgets.Label{text = "Exposure"}, scroll_exposure)
	bloom_group:append_row(Widgets.Label{text = "Influence"}, scroll_luminance)
	local mouse_group = Widget{cols = 2}
	mouse_group:append_row(Widgets.Label{text = "Sensitivity"}, clss.scroll_mouse)
	local sound_group = Widget{cols = 2}
	sound_group:append_row(Widgets.Label{text = "Music"}, clss.scroll_music)
	sound_group:append_row(Widgets.Label{text = "Effects"}, clss.scroll_sound)
	clss.group = Widget{cols = 2}
	clss.group:append_row(Widgets.Label{text = "Quality", font = "medium"})
	clss.group:append_row(quality_group)
	clss.group:append_row(Widgets.Label{text = "Bloom", font = "medium"})
	clss.group:append_row(bloom_group)
	clss.group:append_row(Widgets.Label{text = "Mouse", font = "medium"})
	clss.group:append_row(mouse_group)
	clss.group:append_row(Widgets.Label{text = "Volume", font = "medium"})
	clss.group:append_row(sound_group)
	-- Load the options.
	clss.db = Database{name = "options.sqlite"}
	clss.db:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT PRIMARY KEY,value TEXT);")
	clss:load()
end

--- Saves the options after a delay.<br/>
-- This function is called every time an option is changed. It sets a timer
-- that will save the options to the database after a small delay. The delay
-- exists just in case something goes wrong and the new options cause the game
-- to crash shortly.
-- @param clss Options class.
Options.changed = function(clss)
	if clss.changed_timer ~= nil then
		clss.changed_timer:disable()
	end
	clss.changed_timer = Timer{delay = 3, func = function(timer)
		clss:save()
		timer:disable()
	end}
end

Options.load = function(clss)
	local opts = {
		animation_quality = function(v) clss:set_animation_quality(tonumber(v)) end,
		fullscreen = function(v) clss.fullscreen = (v == "true") end,
		model_quality = function(v) clss:set_model_quality(tonumber(v)) end,
		mouse_sensitivity = function(v) clss:set_mouse_sensitivity(tonumber(v)) end,
		multisamples = function(v) clss:set_multisamples(tonumber(v)) end,
		music_volume = function(v) clss:set_music_volume(tonumber(v)) end,
		sound_volume = function(v) clss:set_sound_volume(tonumber(v)) end,
		transparency_quality = function(v) clss:set_transparency_quality(tonumber(v)) end,
		vsync = function(v) clss.vsync = (v == "true") end,
		window_height = function(v) clss.window_height = tonumber(v) end,
		window_width = function(v) clss.window_width = tonumber(v) end}
	-- Read values from the database.
	local rows = clss.db:query("SELECT key,value FROM keyval;")
	for k,v in ipairs(rows) do
		local opt = opts[v[1]]
		if opt then opt(v[2]) end
	end
	-- Set the video mode.
	clss:set_video_mode(clss.window_width, clss.window_height, clss.fullscreen)
	clss.button_vsync.text = clss.vsync and "Enabled" or "Disabled"
	if clss.fullscreen then
		for k,v in ipairs(clss.video_modes) do
			if v.width == clss.window_width and v.height == clss.window_height then
				clss.combo_video_mode:activate{index = k, press = false}
				break
			end
		end
	else
		clss.combo_video_mode:activate{index = 1, press = false}
	end
end

Options.save = function(clss)
	local write = function(k, v)
		clss.db:query("REPLACE INTO keyval (key,value) VALUES (?,?);", {k, tostring(v)})
	end
	write("animation_quality", clss.animation_quality)
	write("fullscreen", clss.fullscreen)
	write("model_quality", clss.model_quality)
	write("mouse_sensitivity", clss.mouse_sensitivity)
	write("multisamples", clss.multisamples)
	write("music_volume", clss.music_volume)
	write("sound_volume", clss.sound_volume)
	write("transparency_quality", clss.transparency_quality)
	write("vsync", clss.vsync)
	write("window_height", clss.window_height)
	write("window_width", clss.window_width)
end

Options.set_animation_quality = function(clss, v)
	clss.animation_quality = v
	clss.scroll_animation.value = v
	clss:changed()
end

Options.set_model_quality = function(clss, v)
	if v == 1 then
		clss.model_quality = 1
		clss.button_model_quality.text = "High quality"
	else
		clss.model_quality = 0
		clss.button_model_quality.text = "Low quality"
	end
	for i,o in pairs(Object.objects) do
		o:update_model()
	end
	clss:changed()
end

Options.set_mouse_sensitivity = function(clss, v)
	clss.scroll_mouse.value = v
	clss.mouse_sensitivity = v
	clss:changed()
end

Options.set_multisamples = function(clss, v)
	clss.scroll_multisamples.value = math.log(v) / math.log(2)
	clss.scroll_multisamples.text = tonumber(v) .. "x"
	clss.multisamples = v
	clss:changed()
end

Options.set_music_volume = function(clss, v)
	clss.scroll_music.value = v
	clss.music_volume = v
	Sound.music_volume = v
	clss:changed()
end

Options.set_sound_volume = function(clss, v)
	clss.scroll_sound.value = v
	clss.sound_volume = v
	clss:changed()
end

Options.set_transparency_quality = function(clss, v)
	clss.transparency_quality = v
	clss.scroll_transparency.value = v
	clss:changed()
end

Options.set_video_mode = function(clss, w, h, f)
	Client:set_video_mode(w, h, f, clss.vsync)
	local mode = Client.video_mode
	clss.window_width = mode[1]
	clss.window_height = mode[2]
	clss.fullscreen = mode[3]
	clss.vsync = mode[4]
	clss:changed()
end

Options.set_vsync = function(clss, v)
	local mode = Client.video_mode
	clss.window_width = mode[1]
	clss.window_height = mode[2]
	clss.fullscreen = mode[3]
	clss.vsync = v
	clss:set_video_mode(clss.window_width, clss.window_height, clss.fullscreen)
	clss.button_vsync.text = clss.vsync and "Enabled" or "Disabled"
end

Options.toggle = function(clss)
	Gui.menus:open{widget = clss.group}
end

Options:init()

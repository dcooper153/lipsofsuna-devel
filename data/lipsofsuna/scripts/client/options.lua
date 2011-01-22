Options = Class()
Options.fullscreen = false
Options.screen_width = 1024
Options.screen_height = 768
Options.low_quality_models = false
Options.animation_quality = 1.0
Options.mouse_sensitivity = 1.0
Options.multisamples = 2
Options.transparency_quality = 0.3
Options.sound_volume = 1.0
Options.music_volume = 0.3

Options.init = function(clss)
	-- Video mode.
	local modes = {}
	for k,v in ipairs(Client.video_modes) do
		table.insert(modes, {"" .. v[1] .. "x" .. v[2], function()
			Client:set_video_mode(v[1], v[2], true)
		end, width = v[1], height = v[2]})
	end
	table.sort(modes, function(a, b)
		if a.height < b.height then return true end
		if a.height > b.height then return false end
		if a.width < b.width then return true end
		return false
	end)
	table.insert(modes, 1, {"Windowed", function()
		local s = Client.video_mode
		Client:set_video_mode(s[1], s[2], false)
	end})
	local combo_video_mode = Widgets.ComboBox(modes)
	combo_video_mode:activate{index = 1, pressed = false}
	-- Antialiasing quality.
	local samples = math.log(clss.multisamples) / math.log(2)
	local scroll_multisamples = Widgets.Progress{min = 0, max = 4, text = tostring(clss.multisamples) .. "x", value = samples}
	scroll_multisamples:set_request{width = 100}
	scroll_multisamples.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		local p = 2 ^ math.floor(v + 0.5)
		self.value = v
		self.text = p .. "x"
		clss.multisamples = p
	end
	-- Model quality adjustment.
	local button_model_quality = Widgets.Button{text = "High quality"}
	button_model_quality.pressed = function(self)
		if self.text == "Low quality" then
			clss.low_quality_models = false
			self.text = "High quality"
		else
			clss.low_quality_models = true
			self.text = "Low quality"
		end
		for i,o in pairs(Object.objects) do
			o:update_model()
		end
	end
	-- Animation quality adjustment.
	local scroll_animation = Widgets.Progress{min = 0, max = 1, value = clss.animation_quality}
	scroll_animation:set_request{width = 100}
	scroll_animation.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		clss.animation_quality = v
		self.value = v
	end
	-- Transparency quality adjustment.
	local scroll_transparency = Widgets.Progress{min = 0, max = 1, value = clss.transparency_quality}
	scroll_transparency:set_request{width = 100}
	scroll_transparency.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		clss.transparency_quality = v
		self.value = v
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
	end
	-- Luminance adjustment.
	local scroll_luminance = Widgets.Progress{min = 0, max = 1, value = 0}
	scroll_luminance:set_request{width = 100}
	scroll_luminance.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		self.value = v
		Bloom.luminance = 1 - v
		Bloom:compile()
	end
	-- Radius adjustment.
	local scroll_radius = Widgets.Progress{value = 40}
	scroll_radius:set_request{width = 100}
	scroll_radius.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		self.value = v
		Bloom.radius = math.floor(v)
		Bloom:compile()
	end
	-- Exposure adjustment.
	local scroll_exposure = Widgets.Progress{min = 1, max = 10, value = 1.5}
	scroll_exposure:set_request{width = 100}
	scroll_exposure.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		self.value = v
		Bloom.exposure = v
		Bloom:compile()
	end
	-- Mouse sensitivity.
	local scroll_mouse = Widgets.Progress{min = 0, max = 2, value = clss.mouse_sensitivity}
	scroll_mouse:set_request{width = 100}
	scroll_mouse.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		self.value = v
		clss.mouse_sensitivity = v
	end
	-- Sound settings.
	local scroll_music = Widgets.Progress{min = 0, max = 1, value = clss.music_volume}
	scroll_music:set_request{width = 100}
	scroll_music.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		self.value = v
		clss.music_volume = v
		Sound.music_volume = v
	end
	local scroll_sound = Widgets.Progress{min = 0, max = 1, value = clss.sound_volume}
	scroll_sound:set_request{width = 100}
	scroll_sound.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		self.value = v
		clss.sound_volume = v
	end
	-- Packing.
	local quality_group = Widget{cols = 2}
	quality_group:append_row(Widgets.Label{text = "Resolution"}, combo_video_mode)
	quality_group:append_row(Widgets.Label{text = "Models"}, button_model_quality)
	quality_group:append_row(Widgets.Label{text = "Antialiasing"}, scroll_multisamples)
	quality_group:append_row(Widgets.Label{text = "Animation"}, scroll_animation)
	quality_group:append_row(Widgets.Label{text = "Transparency"}, scroll_transparency)
	local bloom_group = Widget{cols = 2}
	bloom_group:append_row(Widgets.Label{text = "Bloom"}, button_bloom)
	bloom_group:append_row(Widgets.Label{text = "Radius"}, scroll_radius)
	bloom_group:append_row(Widgets.Label{text = "Exposure"}, scroll_exposure)
	bloom_group:append_row(Widgets.Label{text = "Influence"}, scroll_luminance)
	local mouse_group = Widget{cols = 2}
	mouse_group:append_row(Widgets.Label{text = "Sensitivity"}, scroll_mouse)
	local sound_group = Widget{cols = 2}
	sound_group:append_row(Widgets.Label{text = "Music"}, scroll_music)
	sound_group:append_row(Widgets.Label{text = "Effects"}, scroll_sound)
	clss.group = Widget{cols = 2}
	clss.group:append_row(Widgets.Label{text = "Quality", font = "medium"})
	clss.group:append_row(quality_group)
	clss.group:append_row(Widgets.Label{text = "Bloom", font = "medium"})
	clss.group:append_row(bloom_group)
	clss.group:append_row(Widgets.Label{text = "Mouse", font = "medium"})
	clss.group:append_row(mouse_group)
	clss.group:append_row(Widgets.Label{text = "Volume", font = "medium"})
	clss.group:append_row(sound_group)
end

Options.toggle = function(clss)
	Gui.menus:open{widget = clss.group}
end

Options:init()

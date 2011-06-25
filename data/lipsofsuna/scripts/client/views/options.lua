Views.Options = Class(Widget)
Views.Options.mode = "options"

Views.Options.new = function(clss)
	-- Allocate self.
	local self = Widget.new(clss, {cols = 2, rows = 4, spacings = {5,0}})
	-- Create a sorted list of video modes.
	-- Some of the views don't fit in the screen unless the height is at least
	-- 720 pixels so we filter out any modes smaller than that.
	self.video_modes = {}
	for k,v in ipairs(Program.video_modes) do
		if v[2] >= 720 then
			table.insert(self.video_modes, {"" .. v[1] .. "x" .. v[2], function()
				self:set_video_mode(v[1], v[2], true)
			end, width = v[1], height = v[2]})
		end
	end
	table.sort(self.video_modes, function(a, b)
		if a.height < b.height then return true end
		if a.height > b.height then return false end
		if a.width < b.width then return true end
		return false
	end)
	table.insert(self.video_modes, 1, {"Windowed", function()
		local s = Program.video_mode
		self:set_video_mode(s[1], s[2], false)
	end})
	-- Video mode.
	self.combo_video_mode = Widgets.ComboBox(self.video_modes)
	self.combo_video_mode:activate{index = 1, press = false}
	-- Antialiasing quality.
	self.scroll_multisamples = Widgets.Progress{min = 0, max = 4, text = "1x", value = 1}
	self.scroll_multisamples:set_request{width = 100}
	self.scroll_multisamples.pressed = function(widget)
		local v = widget:get_value_at(Program.cursor_position)
		self.multisamples = 2 ^ math.floor(v + 0.5)
	end
	-- Anisotrophy quality.
	self.scroll_anisotrophy = Widgets.Progress{min = 0, max = 16, text = "1x", value = 1}
	self.scroll_anisotrophy:set_request{width = 100}
	self.scroll_anisotrophy.pressed = function(widget)
		local v = widget:get_value_at(Program.cursor_position)
		self.anisotrophic_filter = math.floor(v + 0.5)
	end
	-- Model quality adjustment.
	self.button_model_quality = Widgets.Button{text = "High quality"}
	self.button_model_quality.pressed = function(widget)
		if widget.text == "Low quality" then
			self.model_quality = 1
		else
			self.model_quality = 0
		end
	end
	-- Shader quality.
	self.combo_shader_quality = Widgets.ComboBox{
		{"Low", function() self.shader_quality = 1 end},
		{"Medium", function() self.shader_quality = 2 end},
		{"High", function() self.shader_quality = 3 end}}
	-- VSync enable.
	self.button_vsync = Widgets.Button{text = "Disabled"}
	self.button_vsync.pressed = function(widget)
		self.vsync = not self.vsync
	end
	-- Animation quality adjustment.
	self.scroll_animation = Widgets.Progress{min = 0, max = 1, value = 1}
	self.scroll_animation:set_request{width = 100}
	self.scroll_animation.pressed = function(widget)
		local v = widget:get_value_at(Program.cursor_position)
		self.animation_quality = v
	end
	-- Transparency quality adjustment.
	self.scroll_transparency = Widgets.Progress{min = 0, max = 1, value = 1}
	self.scroll_transparency:set_request{width = 100}
	self.scroll_transparency.pressed = function(widget)
		local v = widget:get_value_at(Program.cursor_position)
		self.transparency_quality = v
	end
	-- Bloom toggle.
	self.button_bloom = Widgets.Button{text = "Disabled"}
	self.button_bloom.pressed = function(widget)
		self.bloom_enabled = not self.bloom_enabled
	end
	-- Luminance adjustment.
	self.scroll_luminance = Widgets.Progress{min = 0, max = 1, value = 0}
	self.scroll_luminance:set_request{width = 100}
	self.scroll_luminance.pressed = function(widget)
		local v = widget:get_value_at(Program.cursor_position)
		self.bloom_luminance = 1 - v
	end
	-- Exposure adjustment.
	self.scroll_exposure = Widgets.Progress{min = 1, max = 10, value = 1.5}
	self.scroll_exposure:set_request{width = 100}
	self.scroll_exposure.pressed = function(widget)
		local v = widget:get_value_at(Program.cursor_position)
		self.bloom_exposure = v
	end
	-- Mouse sensitivity.
	self.scroll_mouse = Widgets.Progress{min = 0, max = 2, value = 0.5}
	self.scroll_mouse:set_request{width = 100}
	self.scroll_mouse.pressed = function(widget)
		local v = widget:get_value_at(Program.cursor_position)
		self.mouse_sensitivity = v
	end
	-- Sound settings.
	self.scroll_music = Widgets.Progress{min = 0, max = 0.5, value = 1}
	self.scroll_music:set_request{width = 100}
	self.scroll_music.pressed = function(widget)
		local v = widget:get_value_at(Program.cursor_position) 
		self.music_volume = v
	end
	self.scroll_sound = Widgets.Progress{min = 0, max = 1, value = 1}
	self.scroll_sound:set_request{width = 100}
	self.scroll_sound.pressed = function(widget)
		local v = widget:get_value_at(Program.cursor_position)
		self.sound_volume = v
	end
	-- Packing.
	local quality_group = Widgets.Frame{cols = 2}
	quality_group:append_row(Widgets.Label{text = "Resolution"}, self.combo_video_mode)
	quality_group:append_row(Widgets.Label{text = "Models"}, self.button_model_quality)
	quality_group:append_row(Widgets.Label{text = "Shaders"}, self.combo_shader_quality)
	quality_group:append_row(Widgets.Label{text = "Antialiasing"}, self.scroll_multisamples)
	quality_group:append_row(Widgets.Label{text = "Anisotrophy"}, self.scroll_anisotrophy)
	quality_group:append_row(Widgets.Label{text = "Animation"}, self.scroll_animation)
	quality_group:append_row(Widgets.Label{text = "Transparency"}, self.scroll_transparency)
	quality_group:append_row(Widgets.Label{text = "VSync"}, self.button_vsync)
	local bloom_group = Widgets.Frame{cols = 2}
	bloom_group:append_row(Widgets.Label{text = "Bloom"}, self.button_bloom)
	bloom_group:append_row(Widgets.Label{text = "Influence"}, self.scroll_luminance)
	bloom_group:append_row(Widgets.Label{text = "Exposure"}, self.scroll_exposure)
	local mouse_group = Widgets.Frame{cols = 2}
	mouse_group:append_row(Widgets.Label{text = "Sensitivity"}, self.scroll_mouse)
	local sound_group = Widgets.Frame{cols = 2}
	sound_group:append_row(Widgets.Label{text = "Music"}, self.scroll_music)
	sound_group:append_row(Widgets.Label{text = "Effects"}, self.scroll_sound)
	self:set_child(1, 1, Widgets.Frame{style = "title", text = "Graphics"})
	self:set_child(1, 2, quality_group)
	self:set_child(1, 3, Widgets.Frame{style = "title", text = "Bloom"})
	self:set_child(1, 4, bloom_group)
	self:set_child(2, 1, Widgets.Frame{style = "title", text = "Mouse"})
	self:set_child(2, 2, mouse_group)
	self:set_child(2, 3, Widgets.Frame{style = "title", text = "Volume"})
	self:set_child(2, 4, sound_group)
	-- Load the options.
	self.config = ConfigFile{name = "options.cfg"}
	self:load()
	-- Make sure that shaders are compiled.
	self.shader_quality = self.shader_quality
	return self
end

Views.Options.back = function(self)
	Client:set_mode("menu")
end

--- Saves the options after a delay.<br/>
-- This function is called every time an option is changed. It sets a timer
-- that will save the options to the database after a small delay. The delay
-- exists just in case something goes wrong and the new options cause the game
-- to crash shortly.
-- @param self Options.
Views.Options.changed = function(self)
	if self.changed_timer ~= nil then
		self.changed_timer:disable()
	end
	self.changed_timer = Timer{delay = 3, func = function(timer)
		self:save()
		timer:disable()
	end}
end

Views.Options.load = function(self)
	-- Initialize defaults.
	local vsync = false
	self.fullscreen = false
	self.window_width = 1024
	self.window_height = 768
	self.model_quality = 1
	self.animation_quality = 1
	self.anisotrophic_filter = 0
	self.mouse_sensitivity = 1
	self.multisamples = 2
	self.shader_quality = 2
	self.transparency_quality = 0.3
	self.sound_volume = 1.0
	self.music_volume = 0.1
	-- Read values from the configuration file.
	local opts = {
		animation_quality = function(v) self.animation_quality = tonumber(v) end,
		anisotrophic_filter = function(v) self.anisotrophic_filter = tonumber(v) end,
		bloom = function(v) self.bloom_enabled = (v == "true") end,
		bloom_exposure = function(v) self.bloom_exposure = tonumber(v) end,
		bloom_luminance = function(v) self.bloom_luminance = tonumber(v) end,
		fullscreen = function(v) self.fullscreen = (v == "true") end,
		model_quality = function(v) self.model_quality = tonumber(v) end,
		mouse_sensitivity = function(v) self.mouse_sensitivity = tonumber(v) end,
		multisamples = function(v) self.multisamples = tonumber(v) end,
		music_volume = function(v) self.music_volume = tonumber(v) end,
		shader_quality = function(v) self.shader_quality = tonumber(v) end,
		sound_volume = function(v) self.sound_volume = tonumber(v) end,
		transparency_quality = function(v) self.transparency_quality = tonumber(v) end,
		vsync = function(v) vsync = (v == "true") end,
		window_height = function(v) self.window_height = tonumber(v) end,
		window_width = function(v) self.window_width = tonumber(v) end}
	for k in pairs(opts) do
		local v = self.config:get(k)
		if v then
			local opt = opts[k]
			opt(v)
		end
	end
	-- Set the video mode.
	self:set_video_mode(self.window_width, self.window_height, self.fullscreen, vsync)
	if self.fullscreen then
		for k,v in ipairs(self.video_modes) do
			if v.width == self.window_width and v.height == self.window_height then
				self.combo_video_mode:activate{index = k, press = false}
				break
			end
		end
	else
		self.combo_video_mode:activate{index = 1, press = false}
	end
end

Views.Options.save = function(self)
	local write = function(k, v)
		self.config:set(k, tostring(v))
	end
	write("animation_quality", self.animation_quality)
	write("anisotrophic_filter", self.anisotrophic_filter)
	write("bloom", self.bloom_enabled)
	write("bloom_exposure", self.bloom_exposure)
	write("bloom_luminance", self.bloom_luminance)
	write("fullscreen", self.fullscreen)
	write("model_quality", self.model_quality)
	write("mouse_sensitivity", self.mouse_sensitivity)
	write("multisamples", self.multisamples)
	write("music_volume", self.music_volume)
	write("shader_quality", self.shader_quality)
	write("sound_volume", self.sound_volume)
	write("transparency_quality", self.transparency_quality)
	write("vsync", self.vsync)
	write("window_height", self.window_height)
	write("window_width", self.window_width)
	self.config:save()
end

Views.Options.set_video_mode = function(self, w, h, f)
	Program:set_video_mode(w, h, f, self.vsync)
	local mode = Program.video_mode
	self.window_width = mode[1]
	self.window_height = mode[2]
	self.fullscreen = mode[3]
	self.button_vsync.text = mode[4] and "Enabled" or "Disabled"
	self:changed()
end

Views.Options:add_getters{
	fullscreen = function(self) return rawget(self, "_fullscreen") end,
	animation_quality = function(self) return self.scroll_animation.value end,
	anisotrophic_filter = function(self) return self.scroll_anisotrophy.value end,
	bloom_enabled = function(self) return self.button_bloom.text == "Enabled" end,
	bloom_exposure = function(self) return self.scroll_exposure.value end,
	bloom_luminance = function(self) return 1 - self.scroll_luminance.value end,
	model_quality = function(self) return self.button_model_quality.text == "High quality" end,
	mouse_sensitivity = function(self) return self.scroll_mouse.value end,
	multisamples = function(self) return rawget(self, "_multisamples") end,
	music_volume = function(self) return self.scroll_music.value end,
	shader_quality = function(self) return rawget(self, "_shader_quality") end,
	sound_volume = function(self) return self.scroll_sound.value end,
	transparency_quality = function(self) return self.scroll_transparency.value end,
	vsync = function(self) return self.button_vsync.text == "Enabled" end}

Views.Options:add_setters{
	fullscreen = function(self, value)
		rawset(self, "_fullscreen", value)
	end,
	animation_quality = function(self, v)
		self.scroll_animation.value = v
		self:changed()
	end,
	anisotrophic_filter = function(self, v)
		self.scroll_anisotrophy.value = v
		self.scroll_anisotrophy.text = tonumber(v) .. "x"
		Render.anisotrophy = v
		self:changed()
	end,
	bloom_enabled = function(self, v)
		if v then
			self.button_bloom.text = "Enabled"
		else
			self.button_bloom.text = "Disabled"
		end
		self:changed()
	end,
	bloom_exposure = function(self, v)
		self.scroll_exposure.value = v
		Bloom.exposure = v
		Bloom:compile()
		self:changed()
	end,
	bloom_luminance = function(self, v)
		self.scroll_luminance.value = 1 - v
		Bloom.luminance = v
		Bloom:compile()
		self:changed()
	end,
	model_quality = function(self, v)
		if v == 1 then
			self.button_model_quality.text = "High quality"
		else
			self.button_model_quality.text = "Low quality"
		end
		for i,o in pairs(Object.objects) do
			o:update_model()
		end
		self:changed()
	end,
	mouse_sensitivity = function(self, v)
		self.scroll_mouse.value = v
		self:changed()
	end,
	multisamples = function(self, v)
		rawset(self, "_multisamples", v)
		self.scroll_multisamples.value = math.log(v) / math.log(2)
		self.scroll_multisamples.text = tonumber(v) .. "x"
		self:changed()
	end,
	music_volume = function(self, v)
		self.scroll_music.value = v
		Sound.music_volume = v
		self:changed()
	end,
	shader_quality = function(self, v)
		rawset(self, "_shader_quality", v)
		v = math.min(math.max(v, 1), 3)
		self.combo_shader_quality:activate{index = v, press = false}
		self:changed()
		for k,s in pairs(Shader.dict_name) do
			s:set_quality(v)
		end
	end,
	sound_volume = function(self, v)
		self.scroll_sound.value = v
		self:changed()
	end,
	transparency_quality = function(self, v)
		self.scroll_transparency.value = v
		self:changed()
	end,
	vsync = function(self, v)
		local mode = Program.video_mode
		self.window_width = mode[1]
		self.window_height = mode[2]
		self.fullscreen = mode[3]
		self.button_vsync.text = self.vsync and "Enabled" or "Disabled"
		self:set_video_mode(self.window_width, self.window_height, self.fullscreen, self.vsync)
	end}

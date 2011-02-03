Views.Options = Class(Widget)
Views.Options.mode = "options"

Views.Options.getter = function(self, key, value)
	if key == "fullscreen" then
		return Widget.getter(self, "_fullscreen")
	else
		return Widget.getter(self, key)
	end
end

Views.Options.setter = function(self, key, value)
	if key == "fullscreen" then
		Widget.setter(self, "_fullscreen", value)
	else
		Widget.setter(self, key, value)
	end
end

Views.Options.new = function(clss)
	-- Allocate self.
	local self = Widget.new(clss, {cols = 2})
	self.fullscreen = false
	self.vsync = false
	self.window_width = 1024
	self.window_height = 768
	self.model_quality = 1
	self.animation_quality = 1.0
	self.mouse_sensitivity = 1.0
	self.multisamples = 2
	self.transparency_quality = 0.3
	self.sound_volume = 1.0
	self.music_volume = 0.1
	-- Create a sorted list of video modes.
	self.video_modes = {}
	for k,v in ipairs(Client.video_modes) do
		table.insert(self.video_modes, {"" .. v[1] .. "x" .. v[2], function()
			self:set_video_mode(v[1], v[2], true)
		end, width = v[1], height = v[2]})
	end
	table.sort(self.video_modes, function(a, b)
		if a.height < b.height then return true end
		if a.height > b.height then return false end
		if a.width < b.width then return true end
		return false
	end)
	table.insert(self.video_modes, 1, {"Windowed", function()
		local s = Client.video_mode
		self:set_video_mode(s[1], s[2], false)
	end})
	-- Video mode.
	self.combo_video_mode = Widgets.ComboBox(self.video_modes)
	self.combo_video_mode:activate{index = 1, press = false}
	-- Antialiasing quality.
	local samples = math.log(self.multisamples) / math.log(2)
	self.scroll_multisamples = Widgets.Progress{min = 0, max = 4, text = tostring(self.multisamples) .. "x", value = samples}
	self.scroll_multisamples:set_request{width = 100}
	self.scroll_multisamples.pressed = function(widget)
		local v = widget:get_value_at(Client.cursor_pos)
		self:set_multisamples(2 ^ math.floor(v + 0.5))
	end
	-- Model quality adjustment.
	self.button_model_quality = Widgets.Button{text = "High quality"}
	self.button_model_quality.pressed = function(widget)
		if widget.text == "Low quality" then
			self:set_model_quality(1)
		else
			self:set_model_quality(0)
		end
	end
	-- VSync enable.
	self.button_vsync = Widgets.Button{text = "Disabled"}
	self.button_vsync.pressed = function(widget)
		self:set_vsync(not self.vsync)
	end
	-- Animation quality adjustment.
	self.scroll_animation = Widgets.Progress{min = 0, max = 1, value = self.animation_quality}
	self.scroll_animation:set_request{width = 100}
	self.scroll_animation.pressed = function(widget)
		local v = widget:get_value_at(Client.cursor_pos)
		self:set_animation_quality(v)
	end
	-- Transparency quality adjustment.
	self.scroll_transparency = Widgets.Progress{min = 0, max = 1, value = self.transparency_quality}
	self.scroll_transparency:set_request{width = 100}
	self.scroll_transparency.pressed = function(widget)
		local v = widget:get_value_at(Client.cursor_pos)
		self:set_transparency_quality(v)
	end
	-- Bloom toggle.
	local button_bloom = Widgets.Button{text = "Bloom disabled"}
	button_bloom.pressed = function(widget)
		if widget.text == "Bloom disabled" then
			self.bloom_enabled = true
			widget.text = "Bloom enabled"
		else
			self.bloom_enabled = nil
			widget.text = "Bloom disabled"
		end
		self:changed()
	end
	-- Luminance adjustment.
	local scroll_luminance = Widgets.Progress{min = 0, max = 1, value = 0}
	scroll_luminance:set_request{width = 100}
	scroll_luminance.pressed = function(widget)
		local v = widget:get_value_at(Client.cursor_pos)
		widget.value = v
		Bloom.luminance = 1 - v
		Bloom:compile()
		self:changed()
	end
	-- Radius adjustment.
	local scroll_radius = Widgets.Progress{value = 40}
	scroll_radius:set_request{width = 100}
	scroll_radius.pressed = function(widget)
		local v = widget:get_value_at(Client.cursor_pos)
		widget.value = v
		Bloom.radius = math.floor(v)
		Bloom:compile()
		self:changed()
	end
	-- Exposure adjustment.
	local scroll_exposure = Widgets.Progress{min = 1, max = 10, value = 1.5}
	scroll_exposure:set_request{width = 100}
	scroll_exposure.pressed = function(widget)
		local v = widget:get_value_at(Client.cursor_pos)
		widget.value = v
		Bloom.exposure = v
		Bloom:compile()
		self:changed()
	end
	-- Mouse sensitivity.
	self.scroll_mouse = Widgets.Progress{min = 0, max = 2, value = self.mouse_sensitivity}
	self.scroll_mouse:set_request{width = 100}
	self.scroll_mouse.pressed = function(widget)
		local v = widget:get_value_at(Client.cursor_pos)
		self:set_mouse_sensitivity(v)
	end
	-- Sound settings.
	self.scroll_music = Widgets.Progress{min = 0, max = 0.5, value = self.music_volume}
	self.scroll_music:set_request{width = 100}
	self.scroll_music.pressed = function(widget)
		local v = widget:get_value_at(Client.cursor_pos) 
		self:set_music_volume(v)
	end
	self.scroll_sound = Widgets.Progress{min = 0, max = 1, value = self.sound_volume}
	self.scroll_sound:set_request{width = 100}
	self.scroll_sound.pressed = function(widget)
		local v = widget:get_value_at(Client.cursor_pos)
		self:set_sound_volume(v)
	end
	-- Packing.
	local quality_group = Widget{cols = 2}
	quality_group:append_row(Widgets.Label{text = "Resolution"}, self.combo_video_mode)
	quality_group:append_row(Widgets.Label{text = "Models"}, self.button_model_quality)
	quality_group:append_row(Widgets.Label{text = "Antialiasing"}, self.scroll_multisamples)
	quality_group:append_row(Widgets.Label{text = "Animation"}, self.scroll_animation)
	quality_group:append_row(Widgets.Label{text = "Transparency"}, self.scroll_transparency)
	quality_group:append_row(Widgets.Label{text = "VSync"}, self.button_vsync)
	local bloom_group = Widget{cols = 2}
	bloom_group:append_row(Widgets.Label{text = "Bloom"}, button_bloom)
	bloom_group:append_row(Widgets.Label{text = "Radius"}, scroll_radius)
	bloom_group:append_row(Widgets.Label{text = "Exposure"}, scroll_exposure)
	bloom_group:append_row(Widgets.Label{text = "Influence"}, scroll_luminance)
	local mouse_group = Widget{cols = 2}
	mouse_group:append_row(Widgets.Label{text = "Sensitivity"}, self.scroll_mouse)
	local sound_group = Widget{cols = 2}
	sound_group:append_row(Widgets.Label{text = "Music"}, self.scroll_music)
	sound_group:append_row(Widgets.Label{text = "Effects"}, self.scroll_sound)
	self:append_row(Widgets.Label{text = "Quality", font = "medium"})
	self:append_row(quality_group)
	self:append_row(Widgets.Label{text = "Bloom", font = "medium"})
	self:append_row(bloom_group)
	self:append_row(Widgets.Label{text = "Mouse", font = "medium"})
	self:append_row(mouse_group)
	self:append_row(Widgets.Label{text = "Volume", font = "medium"})
	self:append_row(sound_group)
	-- Load the options.
	self.db = Database{name = "options.sqlite"}
	self.db:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT PRIMARY KEY,value TEXT);")
	self:load()
	return self
end

Views.Options.back = function(self)
	Gui:set_mode("menu")
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
	local opts = {
		animation_quality = function(v) self:set_animation_quality(tonumber(v)) end,
		fullscreen = function(v) self.fullscreen = (v == "true") end,
		model_quality = function(v) self:set_model_quality(tonumber(v)) end,
		mouse_sensitivity = function(v) self:set_mouse_sensitivity(tonumber(v)) end,
		multisamples = function(v) self:set_multisamples(tonumber(v)) end,
		music_volume = function(v) self:set_music_volume(tonumber(v)) end,
		sound_volume = function(v) self:set_sound_volume(tonumber(v)) end,
		transparency_quality = function(v) self:set_transparency_quality(tonumber(v)) end,
		vsync = function(v) self.vsync = (v == "true") end,
		window_height = function(v) self.window_height = tonumber(v) end,
		window_width = function(v) self.window_width = tonumber(v) end}
	-- Read values from the database.
	local rows = self.db:query("SELECT key,value FROM keyval;")
	for k,v in ipairs(rows) do
		local opt = opts[v[1]]
		if opt then opt(v[2]) end
	end
	-- Set the video mode.
	self:set_video_mode(self.window_width, self.window_height, self.fullscreen, self.vsync)
	self.button_vsync.text = self.vsync and "Enabled" or "Disabled"
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
		self.db:query("REPLACE INTO keyval (key,value) VALUES (?,?);", {k, tostring(v)})
	end
	write("animation_quality", self.animation_quality)
	write("fullscreen", self.fullscreen)
	write("model_quality", self.model_quality)
	write("mouse_sensitivity", self.mouse_sensitivity)
	write("multisamples", self.multisamples)
	write("music_volume", self.music_volume)
	write("sound_volume", self.sound_volume)
	write("transparency_quality", self.transparency_quality)
	write("vsync", self.vsync)
	write("window_height", self.window_height)
	write("window_width", self.window_width)
end

Views.Options.set_animation_quality = function(self, v)
	self.animation_quality = v
	self.scroll_animation.value = v
	self:changed()
end

Views.Options.set_model_quality = function(self, v)
	if v == 1 then
		self.model_quality = 1
		self.button_model_quality.text = "High quality"
	else
		self.model_quality = 0
		self.button_model_quality.text = "Low quality"
	end
	for i,o in pairs(Object.objects) do
		o:update_model()
	end
	self:changed()
end

Views.Options.set_mouse_sensitivity = function(self, v)
	self.scroll_mouse.value = v
	self.mouse_sensitivity = v
	self:changed()
end

Views.Options.set_multisamples = function(self, v)
	self.scroll_multisamples.value = math.log(v) / math.log(2)
	self.scroll_multisamples.text = tonumber(v) .. "x"
	self.multisamples = v
	self:changed()
end

Views.Options.set_music_volume = function(self, v)
	self.scroll_music.value = v
	self.music_volume = v
	Sound.music_volume = v
	self:changed()
end

Views.Options.set_sound_volume = function(self, v)
	self.scroll_sound.value = v
	self.sound_volume = v
	self:changed()
end

Views.Options.set_transparency_quality = function(self, v)
	self.transparency_quality = v
	self.scroll_transparency.value = v
	self:changed()
end

Views.Options.set_video_mode = function(self, w, h, f)
	Client:set_video_mode(w, h, f, self.vsync)
	local mode = Client.video_mode
	self.window_width = mode[1]
	self.window_height = mode[2]
	self.fullscreen = mode[3]
	self.vsync = mode[4]
	self:changed()
end

Views.Options.set_vsync = function(self, v)
	local mode = Client.video_mode
	self.window_width = mode[1]
	self.window_height = mode[2]
	self.fullscreen = mode[3]
	self.vsync = v
	self:set_video_mode(self.window_width, self.window_height, self.fullscreen, self.vsync)
	self.button_vsync.text = self.vsync and "Enabled" or "Disabled"
end

Views.Options.inst = Views.Options()
--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.options
-- @alias Options

local Class = require("system/class")
local ConfigFile = require("system/config-file")
local Render = require("system/render")
local Staticobject = require("core/objects/static")

--- TODO:doc
-- @type Options
local Options = Class("Options")

Options.config_keys = {
	anisotropic_filter = {"Anisotropic filtering", "int", 0, 32},
	bloom_enabled = {"Bloom enabled", "bool"},
	bloom_exposure = {"Bloom exposure", "float", 0, 10},
	bloom_luminance = {"Bloom luminance", "float", 0, 1},
	fullscreen = {"Fullscreen mode", "bool"},
	grab_cursor = {"Grab cursor", "bool"},
	help_messages = {"Show help messages", "bool"},
	host_account = {"Account name", "string"},
	host_admin = {"Admin mode", "bool"},
	host_savefile = {"Savefile", "int", 1, 10},
	invert_mouse = {"Inverted mouse", "bool"},
	join_account = {"Account name", "string"},
	join_address = {"Server address", "string"},
	join_port = {"Server port", "int", 1, 65535},
	landmark_view_distance = {"Landmark view distance", "int", 1, 10},
	master_server = {"Master server", "string"},
	model_quality = {"High quality models", "bool"},
	mouse_sensitivity = {"Mouse sensitivity", "float", 0, 1},
	mouse_smoothing = {"Mouse smoothing", "bool"},
	multisamples = {"Antialiasing", "pow", 0, 6},
	music_volume = {"Music volume", "float", 0, 1},
	nudity_enabled = {"Nudity enabled", "bool"},
	outlines_enabled = {"Render outlines", "bool"},
	shader_quality = {"Shader quality", "int", 1, 3},
	shadow_casting_actors = {"Actors cast shadows", "bool"},
	shadow_casting_items = {"Items cast shadows", "bool"},
	shadow_casting_obstacles = {"Obstacles cast shadows", "bool"},
	sound_volume = {"Effect volume", "float", 0, 1},
	ui_size = {"User interface size", "int", 1, 3},
	view_distance = {"View distance", "float", 10, 1000000},
	view_distance_underground = {"View distance underground", "float", 5, 1000000},
	vsync = {"Vertical sync", "bool"},
	window_height = {"Window height", "int", 32, 65536},
	window_width = {"Window width", "int", 32, 65536}}

Options.new = function(clss)
	-- Initialize defaults.
	local self = Class.new(clss)
	self.vsync = false
	self.fullscreen = false
	self.window_width = 1024
	self.window_height = 768
	self.bloom_enabled = false
	self.grab_cursor = false
	self.model_quality = 1
	self.anisotropic_filter = 0
	self.help_messages = true
	self.host_account = "guest"
	self.host_admin = true
	self.host_savefile = 1
	self.invert_mouse = false
	self.join_account = "guest"
	self.join_address = "localhost"
	self.join_port = 10101
	self.landmark_view_distance = 5
	self.master_server = "http://lipsofsuna.org"
	self.mouse_sensitivity = 1
	self.mouse_smoothing = true
	self.multisamples = 2
	self.nudity_enabled = false
	self.outlines_enabled = true
	self.shader_quality = 3
	self.shadow_casting_actors = true
	self.shadow_casting_items = true
	self.shadow_casting_obstacles = true
	self.sound_volume = 1.0
	self.music_volume = 0.1
	self.ui_size = 2
	self.view_distance = 5000
	self.view_distance_underground = 30
	-- Read values from the configuration file.
	self.config = ConfigFile{name = "options.cfg"}
	for k,t in pairs(clss.config_keys) do
		local v = self.config:get(k)
		if v then
			-- Boolean.
			if t[2] == "bool" then
				if v == "true" then
					self[k] = true
				elseif v == "false" then
					self[k] = false
				end
			-- Float.
			elseif t[2] == "float" then
				local v = tonumber(v)
				if v then
					v = math.max(t[3], v)
					v = math.min(t[4], v)
					self[k] = v
				end
			-- Integer.
			elseif t[2] == "int" then
				local v = tonumber(v)
				if v then
					v = math.floor(v)
					v = math.max(t[3], v)
					v = math.min(t[4], v)
					self[k] = v
				end
			-- Power of two integer.
			elseif t[2] == "pow" then
				local v = tonumber(v)
				if v then
					local vv = 2^t[3]
					for i=t[3],t[4] do
						if v < 2^i then break end
						vv = 2^i
					end
					self[k] = vv
				end
			-- String.
			elseif t[2] == "string" then
				self[k] = v
			end
		end
	end
	self:save()
	return self
end

Options.apply = function(self)
	-- Set the anisotropic filter.
	Render:set_anisotrophy(self.anisotropic_filter)
	-- Set the shader scheme.
	if self.shader_quality == 3 then
		if self.outlines_enabled then
			Render:set_material_scheme("Default")
		else
			Render:set_material_scheme("quality3")
		end
	elseif self.shader_quality == 2 then
		if self.outlines_enabled then
			Render:set_material_scheme("quality2outline")
		else
			Render:set_material_scheme("quality2")
		end
	else
		Render:set_material_scheme("quality1")
	end
	-- Set the bloom pass.
	Render:remove_compositor("bloom1")
	if self.shader_quality > 1 and self.bloom_enabled then
		Render:add_compositor("bloom1")
	end
	-- Set the UI theme.
	if Theme:set_theme(self.ui_size) then
		Ui:restart_state()
	end
	-- Update the render properties of objects.
	for k,v in pairs(Game.objects.objects_by_id) do
		self:apply_object(v)
	end
end

Options.apply_object = function(self, object)
	if object.class == Staticobject then
		if self.landmark_view_distance < 10 then
			object.render:set_render_distance(self.landmark_view_distance * 50)
		else
			object.render:set_render_distance(nil)
		end
		object.render:set_shadow_casting(self.shadow_casting_obstacles)
	elseif object.class == Obstacle then
		object.render:set_shadow_casting(self.shadow_casting_obstacles)
	elseif object.class == Actor then
		object.render:set_shadow_casting(self.shadow_casting_actors)
	else
		object.render:set_shadow_casting(self.shadow_casting_items)
	end
end

Options.save = function(self)
	-- Get the sorted list of options.
	local options = {}
	for k in pairs(self.config_keys) do
		table.insert(options, k)
	end
	table.sort(options)
	-- Update and save the configuration file.
	for k,v in ipairs(options) do
		self.config:set(v, tostring(self[v]))
	end
	self.config:save()
end

return Options



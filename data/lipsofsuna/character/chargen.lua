--- Character generator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module character.chargen
-- @alias Chargen

local Camera = require("system/camera")
local Class = require("system/class")
local Color = require("system/color")
local Json = require("system/json")
local ImageMerger = require("system/image-merger")
local Model = require("system/model")
local ModelBuilder = require("character/model-builder")
local ModelMerger = require("system/model-merger")
local RenderObject = require("system/render-object")
local RenderModel = require("system/render-model")
local RenderUtils = require("lipsofsuna/core/client/render-utils")
local TextureBuilder = require("character/texture-builder")

local scale255 = function(t)
	local res = {}
	for k,v in pairs(t) do res[k] = v * 255 end
	return res
end

--- Character generator.
-- @type Chargen
local Chargen = Class("Chargen")

--- Creates a new character generator.
-- @param clss Chargen class.
-- @return Chargen.
Chargen.new = function(clss)
	local self = Class.new(clss)
	self.char = {}
	self.data = {}
	-- FIXME: Should not be here.
	self.list_races = {
		{"Aer", "aer"},
		{"Android", "android"},
		{"Devora", "devora"},
		{"Kraken", "kraken"},
		{"Wyrm", "wyrm"}}
	return self
end

--- Initializes the character creator.
-- @param self Chargen.
-- @param standalone True to run in standalone mode.
Chargen.init = function(self, standalone)
	-- Initialize the state.
	self.data.standalone = standalone
	self.data.active = true
	self.data.rotation = 0
	self.data.translation = Vector(0.3, 1.8, -2)
	-- Create the object.
	self.data.model_merger = ModelMerger()
	self.data.image_merger = ImageMerger()
	self.data.render = RenderObject()
	self.data.render:set_position(Vector(1, 1, 1))
	self.data.render:set_visible(true)
	self:set_race("aer")
	self.data.update_needed = true
	-- Change the music track.
	Client.effects:switch_music_track("char")
end

--- Resets the character creator.
--
-- Context: Any.
--
-- @param self Chargen.
Chargen.reset = function(self)
	if self.data.render then self.data.render:set_visible(false) end
	self.data = {}
	self.char = {
		animation_profile = "default",
		body = {},
		eye_color = {1,1,1},
		eye_style = "aereye1",
		face = {},
		hair_color = {1,1,1},
		hair_style = "default",
		head_style = "aerhead1",
		height = 1,
		name = "Guest",
		race = "aer",
		skin_color = {1,1,1},
		skin_style = "",
		spawn_point = "Home"}
	-- Initialize the body sliders.
	local max_body = 0
	for k,v in ipairs(ChargenSliderSpec:find_by_category("body")) do
		self.char.body[v.field_index] = v.default
		max_body = math.max(max_body, v.field_index)
	end
	for i = 1,max_body do
		if not self.char.body[i] then
			self.char.body[i] = 0
		end
	end
	-- Initialize the face sliders.
	local max_face = 0
	for k,v in ipairs(ChargenSliderSpec:find_by_category("face")) do
		self.char.face[v.field_index] = v.default
		max_face = math.max(max_face, v.field_index)
	end
	for i = 1,max_face do
		if not self.char.face[i] then
			self.char.face[i] = 0
		end
	end
end

--- Sends the character to the server.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
Chargen.apply = function(self)
	if self.data.standalone then return end
	Main.messaging:client_event("create character", {
		animation_profile = self.char.animation_profile,
		body_style = scale255(self.char.body),
		eye_color = scale255(self.char.eye_color),
		eye_style = self.char.eye_style,
		face_style = scale255(self.char.face),
		hair_color = scale255(self.char.hair_color),
		hair_style = self.char.hair_style,
		head_style = self.char.head_style,
		body_scale = self.char.height,
		name = self.char.name,
		race = self.char.race,
		skin_color = scale255(self.char.skin_color),
		skin_style = self.char.skin_style,
		spawn_point = self.char.spawn_point})
end

--- Handles character generator controls.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param args Event arguments.
-- @return True if the caller should handle the event.
Chargen.input = function(self, args)
	if args.type == "mousepress" and args.button == 1 then
		self.data.drag = true
		return false
	elseif args.type == "mouserelease" and args.button == 1 then
		self.data.drag = nil
		return false
	elseif args.type == "mousemotion" and self.data.drag then
		self:rotate(args.dx)
		self:translate(args.dy)
		return false
	end
	return true
end

--- Randomizes the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
Chargen.randomize = function(self)
	local index = math.random(1, #self.list_races)
	self:set_race(self.list_races[index][2])
	self.data.update_needed = true
end

--- Rotates the model of the character creator.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return value Rotation amount.
Chargen.rotate = function(self, value)
	local rad = math.pi * value / 300
	self.data.rotation = self.data.rotation + rad
	local rot = Quaternion{axis = Vector(0, 1, 0), angle = self.data.rotation}
	self.data.render:set_rotation(rot)
end

--- Saves the current character.
-- @param self Chargen.
Chargen.save = function(self)
	-- Set the simple fields.
	local preset = {
		type = "Actorpresetspec",
		animation_profile = self.char.animation_profile,
		body = {},
		eye_color = self.char.eye_color,
		eye_style = self.char.eye_style,
		face = {},
		hair_color = self.char.hair_color,
		hair_style = self.char.hair_style,
		head_style = self.char.head_style,
		skin_color = self.char.skin_color,
		skin_style = self.char.skin_style}
	-- Set the body slider states.
	for k,v in ipairs(ChargenSliderSpec:find_by_category("body")) do
		local val = self.char.body[v.field_index]
		if val then
			preset.body[v.name] = val
		end
	end
	-- Set the face slider states.
	for k,v in ipairs(ChargenSliderSpec:find_by_category("face")) do
		local val = self.char.face[v.field_index]
		if val then
			preset.face[v.name] = val
		end
	end
	-- TODO: Save to a file
	print(Json:encode(preset))
end

--- Translates the model of the character creator.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return value Translation amount.
Chargen.translate = function(self, value)
	local y = self.data.translation.y + value / 300
	self.data.translation.y = math.min(math.max(y, 1), 2)
end

--- Updates the character generator.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param secs Seconds since the last update.
Chargen.update = function(self, secs)
	if not self.data.active then return end
	Client.camera_manager:set_camera_mode("chargen")
	local spec = Actorspec:find{name = self.char.race .. "-player"}
	-- Build models and textures.
	if self.data.update_needed then
		local data = self:get_build_data()
		self.data.model_build_hash = ModelBuilder:build_with_merger(self.data.model_merger, data, self.data.model_build_hash)
		self.data.texture_build_hash = TextureBuilder:build_with_merger(self.data.image_merger, data, self.data.texture_build_hash)
		self.data.update_needed = nil
	end
	-- Apply models.
	local model = self.data.model_merger:pop_model()
	if model then
		-- Set the new model.
		local r = RenderModel(model)
		if self.data.render.model then
			self.data.render:replace_model(self.data.render.model, r)
		else
			self.data.render:add_model(r)
		end
		self.data.render.model = r
		-- Reset the animation.
		local args = spec:get_animation_arguments("idle", self.char.animation_profile)
		self.data.render:animate(args)
		-- Set the body scale.
		local args = RenderUtils:create_scale_animation(spec, self.char.height)
		if args then self.data.render:animate(args) end
	end
	-- Apply textures.
	if self.data.render then
		local image = self.data.image_merger:pop_image()
		if image then
			self.data.render:add_texture_alias("aer1", image) -- FIXME
		end
	end
	-- Update lighting.
	Client.lighting:update(secs)
end

--- Gets the animation profile of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Profile name.
Chargen.get_animation_profile = function(self)
	return self.char.animation_profile
end

--- Sets the animation profile of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param value Profile name.
Chargen.set_animation_profile = function(self, value)
	self.char.animation_profile = value
	self.data.update_needed = true
end

--- Gets the value of a body slider.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param slider Slider index.
-- @return Slider value.
Chargen.get_body = function(self, slider)
	return self.char.body[slider]
end

--- Sets the value of a body slider.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param slider Slider index.
-- @param value Slider value.
Chargen.set_body = function(self, slider, value)
	self.char.body[slider] = value
	self.data.update_needed = true
end

--- Gets the data form model and texture builders.
-- @param self Chargen.
-- @return Table.
Chargen.get_build_data = function(self)
	local spec = Actorspec:find{name = self.char.race .. "-player"}
	return {
		beheaded = false,
		body_scale = self.char.height,
		body_style = scale255(self.char.body),
		equipment = {"bloomers", "bloomers top"},
		eye_color = self.char.eye_color,
		eye_style = self.char.eye_style,
		face_style = scale255(self.char.face),
		hair_color = self.char.hair_color,
		hair_style = self.char.hair_style,
		head_style = self.char.head_style,
		nudity = Client.options.nudity_enabled,
		skin_color = self.char.skin_color,
		skin_style = self.char.skin_style,
		spec = spec}
end

--- Gets the camera focus position.
-- @param self Chargen.
-- @return Vector if active. Nil otherwise.
Chargen.get_camera_focus = function(self)
	if not self.data.active then return end
	return self.data.render:get_position() + self.data.translation
end

--- Gets the eye style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Style name.
Chargen.get_eye_style = function(self)
	return self.char.eye_style
end

--- Sets the eye style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param value Style name.
Chargen.set_eye_style = function(self, value)
	self.char.eye_style = value
	self.data.update_needed = true
end

--- Gets one of the HSV channels of the eye color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param channel Channel index.
-- @return Channel value.
Chargen.get_eye_color = function(self, channel)
	return self.char.eye_color[channel]
end

--- Sets one of the HSV channels of the eye color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param channel Channel index.
-- @param value Channel value.
Chargen.set_eye_color = function(self, channel, value)
	self.char.eye_color[channel] = value
	self.data.update_needed = true
end

--- Gets the value of a face slider.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param slider Slider index.
-- @return Slider value.
Chargen.get_face = function(self, slider)
	return self.char.face[slider]
end

--- Sets the value of a face slider.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param slider Slider index.
-- @param value Slider value.
Chargen.set_face = function(self, slider, value)
	self.char.face[slider] = value
	self.data.update_needed = true
end

--- Gets the hair style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Style name.
Chargen.get_hair_style = function(self)
	return self.char.hair_style
end

--- Sets the hair style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param value Style name.
Chargen.set_hair_style = function(self, value)
	self.char.hair_style = value
	self.data.update_needed = true
end

--- Gets one of the HSV channels of the hair color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param channel Channel index.
-- @return Channel value.
Chargen.get_hair_color = function(self, channel)
	return self.char.hair_color[channel]
end

--- Sets one of the HSV channels of the hair color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param channel Channel index.
-- @param value Channel value.
Chargen.set_hair_color = function(self, channel, value)
	self.char.hair_color[channel] = value
	self.data.update_needed = true
end

--- Gets the head style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Style name.
Chargen.get_head_style = function(self)
	return self.char.head_style
end

--- Sets the head style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param value Style name.
Chargen.set_head_style = function(self, value)
	self.char.head_style = value
	self.data.update_needed = true
end

--- Gets the height of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Height.
Chargen.get_height = function(self, value)
	return self.char.height
end

--- Sets the height of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param value Height.
Chargen.set_height = function(self, value)
	self.char.height = value
	self.data.update_needed = true
end

--- Gets the name of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Name.
Chargen.get_name = function(self)
	return self.char.name
end

--- Sets the name of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param value Name.
Chargen.set_name = function(self, value)
	self.char.name = value
end

--- Sets the character from a preset.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param spec Preset spec.
Chargen.set_preset = function(self, spec)
	if not spec then return end
	for k,v in pairs(spec:get_chargen()) do
		if type(v) == "table" then
			local t = {}
			for k1,v1 in pairs(v) do t[k1] = v1 end
			self.char[k] = t
		else
			self.char[k] = v
		end
	end
	if not spec.animation_profile then
		self.char.animation_profile = "default"
	end
	self.data.update_needed = true
end

--- Gets the list of available presets.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Indexed list of presets.
Chargen.get_presets = function(self)
	local presets = {}
	for k,v in pairs(Actorpresetspec.dict_name) do
		if v.playable then
			table.insert(presets, v)
		end
	end
	table.sort(presets, function(a,b) return a.name < b.name end)
	return presets
end

--- Gets the name of the active race.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Race name.
Chargen.get_race = function(self)
	return self.char.race
end

--- Sets the name of the active race.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param race Race name.
Chargen.set_race = function(self, race)
	-- Set the race selection.
	self.char.race = race
	-- Choose a random preset.
	local presets = self:get_presets()
	local preset = presets[math.random(1,#presets)]
	self:set_preset(preset)
end

--- Gets the list of available races.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Indexed list of races.
Chargen.get_races = function(self)
	return self.list_races
end

--- Gets the skin style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Skin style name.
Chargen.get_skin_style = function(self)
	return self.char.skin_style
end

--- Sets the skin style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param value Style name.
Chargen.set_skin_style = function(self, value)
	self.char.skin_style = value
	self.data.update_needed = true
end

--- Gets one of the HSV channels of the skin color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param channel Channel index.
-- @return Channel value.
Chargen.get_skin_color = function(self, channel)
	return self.char.skin_color[channel]
end

--- Sets one of the HSV channels of the skin color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param channel Channel index.
-- @param value Channel value.
Chargen.set_skin_color = function(self, channel, value)
	self.char.skin_color[channel] = value
	self.data.update_needed = true
end

--- Gets the spawn point of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Spawn point name.
Chargen.get_spawn_point = function(self, channel)
	return self.char.spawn_point
end

--- Sets the spawn point of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param value Spawn point name.
Chargen.set_spawn_point = function(self, value)
	self.char.spawn_point = value
end

return Chargen

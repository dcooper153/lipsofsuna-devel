local ModelBuilder = require(Mod.path .. "model-builder")
local RenderUtils = require(Mod.path .. "render-utils")

local scale255 = function(t)
	local res = {}
	for k,v in pairs(t) do res[k] = v * 255 end
	return res
end

Operators.chargen = Class()
Operators.chargen.char = {}
Operators.chargen.data = {}

-- FIXME: Should not be here.
Operators.chargen.list_races = {
	{"Aer", "aer"},
	{"Android", "android"},
	{"Devora", "devora"},
	{"Kraken", "kraken"},
	{"Wyrm", "wyrm"}}

--- Initializes the character creator.
--
-- Context: Any.
--
-- @param self Operator.
Operators.chargen.init = function(self)
	-- Create the object.
	self.data.merger = Merger()
	self.data.render = RenderObject()
	self.data.render:set_position(Vector(1, 1, 1))
	self.data.render:set_visible(true)
	self:randomize()
	-- Create the camera.
	self.data.translation = Vector(0.3, 1.8, -2)
	self.data.camera = Camera{far = 60.0, near = 0.3, mode = "first-person"}
	self.data.camera:warp()
	self:update(0.0)
	Client.camera = self.data.camera
	-- Change the music track.
	Client.effects:switch_music_track("char")
end

--- Resets the character creator.
--
-- Context: Any.
--
-- @param self Operator.
Operators.chargen.reset = function(self)
	if self.data.render then self.data.render:set_visible(false) end
	self.data = {}
	self.char = {
		animation_profile = "default",
		body = {0,0,0,0,0,0,0,0,0,0},
		eye_color = {1,1,1},
		eye_style = "default",
		face = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		hair_color = {1,1,1},
		hair_style = "default",
		head_style = "aerhead1",
		height = 1,
		name = "Guest",
		race = "aer",
		skin_color = {1,1,1},
		skin_style = "",
		spawn_point = "Home"}
end

--- Sends the character to the server.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
Operators.chargen.apply = function(self)
	Game.messaging:client_event("create character", {
		animation_profile = self.char.animation_profile,
		body_style = scale255(self.char.body),
		eye_color = scale255(Color:hsv_to_rgb(self.char.eye_color)),
		eye_style = self.char.eye_style,
		face_style = scale255(self.char.face),
		hair_color = scale255(Color:hsv_to_rgb(self.char.hair_color)),
		hair_style = self.char.hair_style,
		head_style = self.char.head_style,
		body_scale = self.char.height,
		name = self.char.name,
		race = self.char.race,
		skin_color = scale255(Color:hsv_to_rgb(self.char.skin_color)),
		skin_style = self.char.skin_style,
		spawn_point = self.char.spawn_point})
end

--- Handles character generator controls.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param args Event arguments.
-- @return True if the caller should handle the event.
Operators.chargen.input = function(self, args)
	if not Ui.pointer_grab then return true end
	local ret = true
	-- Rotate the character.
	local action1 = Client.bindings:find_by_name("turn")
	local response1 = action1 and action1:get_event_response(args)
	if response1 then
		self:rotate(response1)
		ret = false
	end
	-- Move the character.
	local action2 = Client.bindings:find_by_name("tilt")
	local response2 = action2 and action2:get_event_response(args)
	if response2 then
		self:translate(response2)
		ret = false
	end
	return ret
end

--- Randomizes the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
Operators.chargen.randomize = function(self)
	local index = math.random(1, #self.list_races)
	self:set_race(self.list_races[index][2])
	self.data.update_needed = true
end

--- Rotates the model of the character creator.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @return value Rotation amount.
Operators.chargen.rotate = function(self, value)
	local rad = math.pi * value / 300
	local rot = Quaternion{axis = Vector(0, 1, 0), angle = rad}
	self.data.render:set_rotation(self.data.render:get_rotation() * rot)
end

--- Translates the model of the character creator.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @return value Translation amount.
Operators.chargen.translate = function(self, value)
	local y = self.data.translation.y + value / 300
	self.data.translation.y = math.min(math.max(y, 1), 2)
end

--- Updates the character generator.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param secs Seconds since the last update.
Operators.chargen.update = function(self, secs)
	local spec = Actorspec:find{name = self.char.race .. "-player"}
	-- Build models.
	if self.data.update_needed then
		ModelBuilder:build_with_merger(self.data.merger, {
			beheaded = false,
			body_scale = self.char.height,
			body_style = scale255(self.char.body),
			equipment = {},
			eye_color = Color:hsv_to_rgb(self.char.eye_color),
			eye_style = self.char.eye_style,
			face_style = scale255(self.char.face),
			hair_color = Color:hsv_to_rgb(self.char.hair_color),
			hair_style = self.char.hair_style,
			head_style = self.char.head_style,
			nudity = Client.options.nudity_enabled,
			skin_color = Color:hsv_to_rgb(self.char.skin_color),
			skin_style = self.char.skin_style,
			spec = spec})
		self.data.update_needed = nil
	end
	-- Apply models.
	local model = self.data.merger:pop_model()
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
	-- Update the camera.
	self.data.camera.target_position = self.data.render:get_position() + self.data.translation
	self.data.camera.target_rotation = Quaternion{axis = Vector(0, 1, 0), angle = math.pi}
	self.data.camera:update(secs)
	Client:update_camera()
	-- Update lighting.
	Client.lighting:update(secs)
end

--- Gets the animation profile of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @return Profile name.
Operators.chargen.get_animation_profile = function(self)
	return self.char.animation_profile
end

--- Sets the animation profile of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param value Profile name.
Operators.chargen.set_animation_profile = function(self, value)
	self.char.animation_profile = value
	self.data.update_needed = true
end

--- Gets the value of a body slider.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param slider Slider index.
-- @return Slider value.
Operators.chargen.get_body = function(self, slider)
	return self.char.body[slider]
end

--- Sets the value of a body slider.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param slider Slider index.
-- @param value Slider value.
Operators.chargen.set_body = function(self, slider, value)
	self.char.body[slider] = value
	self.data.update_needed = true
end

--- Gets the eye style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @return Style name.
Operators.chargen.get_eye_style = function(self)
	return self.char.eye_style
end

--- Sets the eye style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param value Style name.
Operators.chargen.set_eye_style = function(self, value)
	self.char.eye_style = value
	self.data.update_needed = true
end

--- Gets one of the HSV channels of the eye color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param channel Channel index.
-- @return Channel value.
Operators.chargen.get_eye_color = function(self, channel)
	return self.char.eye_color[channel]
end

--- Sets one of the HSV channels of the eye color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param channel Channel index.
-- @param value Channel value.
Operators.chargen.set_eye_color = function(self, channel, value)
	self.char.eye_color[channel] = value
	self.data.update_needed = true
end

--- Gets the value of a face slider.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param slider Slider index.
-- @return Slider value.
Operators.chargen.get_face = function(self, slider)
	return self.char.face[slider]
end

--- Sets the value of a face slider.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param slider Slider index.
-- @param value Slider value.
Operators.chargen.set_face = function(self, slider, value)
	self.char.face[slider] = value
	self.data.update_needed = true
end

--- Gets the hair style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @return Style name.
Operators.chargen.get_hair_style = function(self)
	return self.char.hair_style
end

--- Sets the hair style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param value Style name.
Operators.chargen.set_hair_style = function(self, value)
	self.char.hair_style = value
	self.data.update_needed = true
end

--- Gets one of the HSV channels of the hair color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param channel Channel index.
-- @return Channel value.
Operators.chargen.get_hair_color = function(self, channel)
	return self.char.hair_color[channel]
end

--- Sets one of the HSV channels of the hair color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param channel Channel index.
-- @param value Channel value.
Operators.chargen.set_hair_color = function(self, channel, value)
	self.char.hair_color[channel] = value
	self.data.update_needed = true
end

--- Gets the head style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @return Style name.
Operators.chargen.get_head_style = function(self)
	return self.char.head_style
end

--- Sets the head style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param value Style name.
Operators.chargen.set_head_style = function(self, value)
	self.char.head_style = value
	self.data.update_needed = true
end

--- Gets the height of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @return Height.
Operators.chargen.get_height = function(self, value)
	return self.char.height
end

--- Sets the height of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param value Height.
Operators.chargen.set_height = function(self, value)
	self.char.height = value
	self.data.update_needed = true
end

--- Gets the name of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @return Name.
Operators.chargen.get_name = function(self)
	return self.char.name
end

--- Sets the name of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param value Name.
Operators.chargen.set_name = function(self, value)
	self.char.name = value
end

--- Sets the character from a preset.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param args Preset table.
Operators.chargen.set_preset = function(self, args)
	for k,v in pairs(args) do
		if k ~= "name" then
			if type(v) == "table" then
				local t = {}
				for k1,v1 in pairs(v) do t[k1] = v1 end
				self.char[k] = t
			else
				self.char[k] = v
			end
		end
	end
	if not args.animation_profile then
		self.char.animation_profile = "default"
	end
	self.data.update_needed = true
end

--- Gets the list of available presets.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @return Indexed list of presets.
Operators.chargen.get_presets = function(self)
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
-- @param self Operator.
-- @return Race name.
Operators.chargen.get_race = function(self)
	return self.char.race
end

--- Sets the name of the active race.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param race Race name.
Operators.chargen.set_race = function(self, race)
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
-- @param self Operator.
-- @return Indexed list of races.
Operators.chargen.get_races = function(self)
	return self.list_races
end

--- Gets the skin style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @return Skin style name.
Operators.chargen.get_skin_style = function(self)
	return self.char.skin_style
end

--- Sets the skin style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param value Style name.
Operators.chargen.set_skin_style = function(self, value)
	self.char.skin_style = value
	self.data.update_needed = true
end

--- Gets one of the HSV channels of the skin color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param channel Channel index.
-- @return Channel value.
Operators.chargen.get_skin_color = function(self, channel)
	return self.char.skin_color[channel]
end

--- Sets one of the HSV channels of the skin color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param channel Channel index.
-- @param value Channel value.
Operators.chargen.set_skin_color = function(self, channel, value)
	self.char.skin_color[channel] = value
	self.data.update_needed = true
end

--- Gets the spawn point of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @return Spawn point name.
Operators.chargen.get_spawn_point = function(self, channel)
	return self.char.spawn_point
end

--- Sets the spawn point of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Operator.
-- @param value Spawn point name.
Operators.chargen.set_spawn_point = function(self, value)
	self.char.spawn_point = value
end

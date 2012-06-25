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
	self.data.object = Actor{position = Vector(1, 1, 1), realized = true}
	self:randomize()
	-- Create the camera.
	self.data.translation = Vector(0.3, 1.8, -2)
	self.data.camera = Camera{far = 60.0, near = 0.3, mode = "first-person"}
	self.data.camera:warp()
	self:update(0.0)
	Client.camera = self.data.camera
	-- Change the music track.
	Sound:switch_music_track("char")
end

--- Resets the character creator.
--
-- Context: Any.
--
-- @param self Operator.
Operators.chargen.reset = function(self)
	if self.data.object then self.data.object:detach() end
	self.data = {}
	self.char = {
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
	local hair_rgb = Color:hsv_to_rgb(self.char.hair_color)
	local eye_rgb = Color:hsv_to_rgb(self.char.eye_color)
	local skin_rgb = Color:hsv_to_rgb(self.char.skin_color)
	local packet = Packet(packets.CHARACTER_CREATE,
		"string", self.char.name,
		"string", self.char.race,
		-- Body style.
		"uint8", 255 * self.char.height,
		"uint8", 255 * self.char.body[1],
		"uint8", 255 * self.char.body[2],
		"uint8", 255 * self.char.body[3],
		"uint8", 255 * self.char.body[4],
		"uint8", 255 * self.char.body[5],
		"uint8", 255 * self.char.body[6],
		"uint8", 255 * self.char.body[7],
		"uint8", 255 * self.char.body[8],
		"uint8", 255 * self.char.body[9],
		"uint8", 255 * self.char.body[10],
		-- Head style.
		"string", self.char.head_style,
		-- Eye style.
		"string", "default",
		"uint8", 255 * eye_rgb[1],
		"uint8", 255 * eye_rgb[2],
		"uint8", 255 * eye_rgb[3],
		-- Face style.
		"uint8", 255 * self.char.face[1],
		"uint8", 255 * self.char.face[2],
		"uint8", 255 * self.char.face[3],
		"uint8", 255 * self.char.face[4],
		"uint8", 255 * self.char.face[5],
		"uint8", 255 * self.char.face[6],
		"uint8", 255 * self.char.face[7],
		"uint8", 255 * self.char.face[8],
		"uint8", 255 * self.char.face[9],
		"uint8", 255 * self.char.face[10],
		"uint8", 255 * self.char.face[11],
		"uint8", 255 * self.char.face[12],
		"uint8", 255 * self.char.face[13],
		"uint8", 255 * self.char.face[14],
		"uint8", 255 * self.char.face[15],
		-- Hair style.
		"string", self.char.hair_style,
		"uint8", 255 * hair_rgb[1],
		"uint8", 255 * hair_rgb[2],
		"uint8", 255 * hair_rgb[3],
		-- Skin style.
		"string", self.char.skin_style,
		"uint8", 255 * skin_rgb[1],
		"uint8", 255 * skin_rgb[2],
		"uint8", 255 * skin_rgb[3],
		-- Spawn point
		"string", self.char.spawn_point)
	Network:send{packet = packet}
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
	local action1 = Binding.dict_name["turn"]
	local response1 = action1 and action1:get_event_response(args)
	if response1 then
		self:rotate(response1)
		ret = false
	end
	-- Move the character.
	local action2 = Binding.dict_name["tilt"]
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
	self.data.object.rotation = self.data.object.rotation * rot
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
	-- Update model.
	if self.data.update_needed then
		local object = self.data.object
		local spec = Actorspec:find{name = self.char.race .. "-player"}
		self.data.update_needed = nil
		object.spec = spec
		object.body_scale = self.char.height
		object.body_style = self.char.body
		object.equipment = {}
		object.eye_color = Color:hsv_to_rgb(self.char.eye_color)
		object.eye_style = nil
		object.face_style = self.char.face
		object.hair_color = Color:hsv_to_rgb(self.char.hair_color)
		object.hair_style = self.char.hair_style
		object.head_style = self.char.head_style
		object.skin_color = Color:hsv_to_rgb(self.char.skin_color)
		object.skin_style = self.char.skin_style
		object:set_model()
		object:add_animation("idle")
		if self.dump_presets then
			print(self.data.object:write_preset())
		end
	end
	self.data.object:refresh()
	-- Update the camera.
	self.data.camera.target_position = self.data.object.position + self.data.translation
	self.data.camera.target_rotation = Quaternion{axis = Vector(0, 1, 0), angle = math.pi}
	self.data.camera:update(secs)
	--self:update_camera()
	Client:update_camera()
	-- Update lighting.
	Lighting:update(secs)
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

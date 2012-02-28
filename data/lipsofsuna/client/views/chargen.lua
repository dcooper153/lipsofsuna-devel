Views.Chargen = Class(Widgets.Scene)
Views.Chargen.class_name = "Views.Chargen"
Views.Chargen.list_races = {
	{"Aer", "aer"},
	{"Android", "android"},
	{"Devora", "devora"},
	{"Kraken", "kraken"},
	{"Wyrm", "wyrm"}}

-- Set to true to dump the settings to the console in a copiable form.
Views.Chargen.dump_presets = false
Views.Chargen.list_presets = {
	{name="Female 1",body={[1]=0,[2]=0.5,[3]=0.5,[4]=0.76635514018692,[5]=0.032710280373832,[6]=0.023364485981308,[7]=0.0046728971962617,[8]=0.55607476635514},skin_color={[1]=1,[2]=0.1,[3]=1},eye_color={[1]=0.70740740740741,[2]=0.97037037037037,[3]=0.49259259259259},height=0.91120294158403,hair_color={[1]=0.70740740740741,[2]=0.5037037037037,[3]=0.34814814814815},face={[1]=0.68303312579311,[2]=0.39000927069691,[3]=0.5925592596608,[4]=0.71896150043186,[5]=0.019047619047619,[6]=0.4952380952381,[7]=0.019047619047619,[8]=0.23809523809524,[9]=0.015353406786618,[10]=0.51618284011082,[11]=0.2371160533452,[12]=0.57142857142857,[13]=0.65562895250303,[14]=0.033333333333333,[15]=0.82454440362032},hair_style="hair4"},
	{name="Female 2",body={[1]=0.2196261682243,[2]=0.5,[3]=0.5,[4]=0.76635514018692,[5]=0.20093457943925,[6]=0.33644859813084,[7]=0.46261682242991,[8]=0.60280373831776},skin_color={[1]=1,[2]=0.1,[3]=1},eye_color={[1]=0.70740740740741,[2]=0.97037037037037,[3]=0.49259259259259},height=0.99065420560748,hair_color={[1]=0.70740740740741,[2]=0.5037037037037,[3]=0.34814814814815},face={[1]=0.31696687420689,[2]=0.64285714285714,[3]=0.5047619047619,[4]=0.78095238095238,[5]=0.50952380952381,[6]=0.41904761904762,[7]=0.019047619047619,[8]=0.37142857142857,[9]=0.25714285714286,[10]=0.48381715988918,[11]=0.48571428571429,[12]=0.57142857142857,[13]=0.65562895250303,[14]=0.33333333333333,[15]=0.82454440362032},hair_style="hair1"},
	{name="Female 3",body={[1]=0.2196261682243,[2]=0.66355140186916,[3]=0.14018691588785,[4]=0.76635514018692,[5]=0.20093457943925,[6]=0.2196261682243,[7]=0.12616822429907,[8]=0.54205607476636},skin_color={[1]=1,[2]=0.1,[3]=1},eye_color={[1]=0.97037037037037,[2]=0.97037037037037,[3]=0.20740740740741},height=1.0485981308411,hair_color={[1]=0.70740740740741,[2]=0.5037037037037,[3]=0.34814814814815},face={[1]=0.30952380952381,[2]=0.50952380952381,[3]=0.69047619047619,[4]=0.62857142857143,[5]=0.31428571428571,[6]=0.41904761904762,[7]=0.1952380952381,[8]=0.061904761904762,[9]=0.25714285714286,[10]=0.48381715988918,[11]=0.15714285714286,[12]=0.57142857142857,[13]=0.51904761904762,[14]=0.0095238095238095,[15]=0.17619047619048},hair_style="hair5"},
	{name="Male 1",body={[1]=0.44392523364486,[2]=0.5607476635514,[3]=0.018691588785047,[4]=0.023364485981308,[5]=0.78504672897196,[6]=0.73831775700935,[7]=0.50934579439252,[8]=0.73364485981308},skin_color={[1]=1,[2]=0.1,[3]=1},eye_color={[1]=0.70740740740741,[2]=0.18888888888889,[3]=0.49259259259259},height=0.91120294158403,hair_color={[1]=0.70740740740741,[2]=0.5037037037037,[3]=0.34814814814815},face={[1]=0.64285714285714,[2]=0.78571428571429,[3]=0.2,[4]=0.48571428571429,[5]=0.12857142857143,[6]=0.5047619047619,[7]=0.17142857142857,[8]=0.50952380952381,[9]=0.42380952380952,[10]=0.43333333333333,[11]=0.63333333333333,[12]=0.57142857142857,[13]=0.77142857142857,[14]=0.68095238095238,[15]=0.24761904761905},hair_style="hair5"},
	{name="Male 2",body={[1]=0.33177570093458,[2]=0.45327102803738,[3]=0.0093457943925234,[4]=0.0093457943925234,[5]=0.36854397941779,[6]=0.91121495327103,[7]=0.18224299065421,[8]=0.88317757009346},skin_color={[1]=1,[2]=0.1,[3]=1},eye_color={[1]=0.98758912830967,[2]=0.50854464038673,[3]=0.50543860742144},height=0.97416166254513,hair_color={[1]=0.19428798472243,[2]=0.68148148148148,[3]=0.81481481481481},face={[1]=0.095238095238095,[2]=0.44285714285714,[3]=0.12857142857143,[4]=0.97711486973665,[5]=0.070011058389214,[6]=0.27460515605035,[7]=0.74761904761905,[8]=0.1952380952381,[9]=0.44866463469745,[10]=0.36289419623227,[11]=0.78943752580762,[12]=0.28424429627333,[13]=0.35238095238095,[14]=0.91790563423089,[15]=0.65714285714286},hair_style="hair5"},
	{name="Male 3",body={[1]=0.63084112149533,[2]=0.42056074766355,[3]=0.018691588785047,[4]=0.023364485981308,[5]=0.78504672897196,[6]=0.97196261682243,[7]=0.13084112149533,[8]=0.77570093457944},skin_color={[1]=1,[2]=0.1,[3]=1},eye_color={[1]=0.95925925925926,[2]=0.57777777777778,[3]=0.25185185185185},height=1.0366804412376,hair_color={[1]=0.36666666666667,[2]=0.53703703703704,[3]=0.9037037037037},face={[1]=0.64285714285714,[2]=0.78571428571429,[3]=0.0047619047619048,[4]=0.84761904761905,[5]=0.12857142857143,[6]=0.35238095238095,[7]=0.17142857142857,[8]=0.63809523809524,[9]=0.42380952380952,[10]=0.25238095238095,[11]=0.5952380952381,[12]=0.15714285714286,[13]=0.9952380952381,[14]=0.98095238095238,[15]=0.24761904761905},hair_style="hair5"}}

--- Creates a new chargen view.
-- @param clss Chargen class.
-- @return Chargen.
Views.Chargen.new = function(clss)
	-- Preview scene.
	local camera = Camera{far = 60.0, near = 0.3, mode = "first-person"}
	local self = Widgets.Scene.new(clss, {rows = 1, fullscreen = true, camera = camera, spacings = {0,0}})

	self.char = {
		body = {0,0,0,0,0,0,0,0},
		eye_color = {1,1,1},
		eye_style = "default",
		face = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		hair_color = {1,1,1},
		hair_style = "default",
		height = 1,
		name = "Guest",
		race = "aer",
		skills = {
			dexterity = 1,
			health = 1,
			intelligence = 1,
			perception = 1,
			strength = 1,
			willpower = 1},
		skin_color = {1,1,1},
		skin_style = "default",
		spawn_point = ""}

	self.view_offset = Vector(0, 1.8, -2)
	self.object = Creature{position = Vector(1, 1, 1)}

	-- Spawn point selector.
--[[	local spawnpoints = {{"Home", function() end}}
	for k,v in ipairs(Regionspec:find_spawn_points()) do
		table.insert(spawnpoints, {v.name, function() end})
	end
	self.label_spawn = Widgets.Label{text = "Spawn point:"}
	self.combo_spawn = Widgets.ComboBox(spawnpoints)
	self.combo_spawn:activate{index = 1}--]]

	return self
end

Views.Chargen.apply = function(self)
	local hair_rgb = Color:hsv_to_rgb(self.char.hair_color)
	local eye_rgb = Color:hsv_to_rgb(self.char.eye_color)
	local skin_rgb = Color:hsv_to_rgb(self.char.skin_color)
	local packet = Packet(packets.CHARACTER_CREATE,
		"string", self.char.name,
		"string", self.char.race,
		-- Skills.
		"uint8", self.char.skills["dexterity"],
		"uint8", self.char.skills["health"],
		"uint8", self.char.skills["intelligence"],
		"uint8", self.char.skills["perception"],
		"uint8", self.char.skills["strength"],
		"uint8", self.char.skills["willpower"],
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

Views.Chargen.close = function(self)
end

--- Executes the character generator.
-- @param self Chargen.
-- @param from Name of the previous mode.
Views.Chargen.enter = function(self, from)
	-- Cleanup if disconnected.
	if from ~= "startup" then
		for k,v in pairs(Object.objects) do v:detach() end
		Player.object = nil
	end
	-- Show the UI.
	self.floating = true
	self.object.realized = true
	self:random()
	self:update(0.0)
	self.camera:warp()
	Sound:switch_music_track("char")
end

Views.Chargen.init = function(self)
	-- Cleanup old objects.
	for k,v in pairs(Object.objects) do v:detach() end
	-- Initialize the display.
	self.object.realized = true
	self:random()
	self:update(0.0)
	self.camera:warp()
	Sound:switch_music_track("char")
end

Views.Chargen.mousemotion = function(self, args)
	if self.dragging then
		local y = self.view_offset.y + args.dy / 300
		self.view_offset.y = math.min(math.max(y, 1), 2)
		self:rotate(math.pi * args.dx / 300)
	end
end

Views.Chargen.pressed = function(self, args)
	if args.button == 1 then
		self.dragging = true
	end
end

Views.Chargen.event = function(self, args)
	if args.type == "mouserelease" then
		if args.button == 1 then
			self.dragging = nil
		end
	end
end

Views.Chargen.quit = function(self)
	Program.quit = true
end

--- Randomizes the character.
-- @param self Chargen.
Views.Chargen.random = function(self)
	local index = math.random(1, #self.list_races)
	self:set_race(self.list_races[index][2])
	self.update_needed = true
end

--- Rotates the character.
-- @param self Chargen.
-- @param rad Radians.
Views.Chargen.rotate = function(self, rad)
	local rot = Quaternion{axis = Vector(0, 1, 0), angle = rad}
	self.object.rotation = self.object.rotation * rot
end

Views.Chargen.get_body = function(self, slider)
	return self.char.body[slider]
end

Views.Chargen.set_body = function(self, slider, value)
	self.char.body[slider] = value
	self.update_needed = true
end

Views.Chargen.set_eye_style = function(self, value)
	self.char.eye_style = value
	self.update_needed = true
end

Views.Chargen.get_eye_color = function(self, channel)
	return self.char.eye_color[channel]
end

Views.Chargen.set_eye_color = function(self, channel, value)
	self.char.eye_color[channel] = value
	self.update_needed = true
end

Views.Chargen.get_face = function(self, slider)
	return self.char.face[slider]
end

Views.Chargen.set_face = function(self, slider, value)
	self.char.face[slider] = value
	self.update_needed = true
end

Views.Chargen.set_hair_style = function(self, value)
	self.char.hair_style = value
	self.update_needed = true
end

Views.Chargen.get_hair_color = function(self, channel)
	return self.char.hair_color[channel]
end

Views.Chargen.set_hair_color = function(self, channel, value)
	self.char.hair_color[channel] = value
	self.update_needed = true
end

Views.Chargen.set_height = function(self, value)
	self.char.height = value
	self.update_needed = true
end

Views.Chargen.get_name = function(self)
	return self.char.name
end

Views.Chargen.set_name = function(self, value)
	self.char.name = value
end

Views.Chargen.get_race = function(self)
	return Species:find{name = self.char.race}
end

Views.Chargen.set_race = function(self, race)
	-- Set the race selection.
	self.char.race = race
	-- Choose a random preset.
	local index = math.random(1, #self.list_presets)
	self:set_preset(self.list_presets[index])
	-- Randomize the name.
	--self.entry_name.text = Names:random{race = self.list_races[self.combo_race.value][2], gender = "female"}
	-- Reset skills.
	--self.skills:set_species(spec)
end

Views.Chargen.set_skin_style = function(self, value)
	self.char.skin_style = value
	self.update_needed = true
end

Views.Chargen.get_skin_color = function(self, channel)
	return self.char.skin_color[channel]
end

Views.Chargen.set_skin_color = function(self, channel, value)
	self.char.skin_color[channel] = value
	self.update_needed = true
end

Views.Chargen.update = function(self, secs)
	-- Update model.
	if self.update_needed then
		self.update_needed = nil
		self:update_model()
	end
	self.object:refresh()
	-- Update the camera.
	self.camera.target_position = self.object.position + self.view_offset
	self.camera.target_rotation = Quaternion{axis = Vector(0, 1, 0), angle = math.pi}
	self.camera:update(secs)
	self:update_camera()
	-- Update lighting.
	Lighting:update(secs)
end

--- Sets the character from a preset.
-- @param self Chargen.
-- @param args Preset table.
Views.Chargen.set_preset = function(self, args)
	for k,v in pairs(args) do
		if k ~= "name" then
			self.char[k] = v
		end
	end
	self.update_needed = true
end

Views.Chargen.update_model = function(self)
	local spec = Species:find{name = self.char.race .. "-player"}
	local items = {}
	self.object.spec = spec
	self.object.body_scale = self.char.height
	self.object.body_style = self.char.body
	self.object.equipment = items
	self.object.eye_color = Color:hsv_to_rgb(self.char.eye_color)
	self.object.eye_style = nil
	self.object.face_style = self.char.face
	self.object.hair_color = Color:hsv_to_rgb(self.char.hair_color)
	self.object.hair_style = self.char.hair_style
	self.object.skin_color = Color:hsv_to_rgb(self.char.skin_color)
	self.object.skin_style = nil
	self.object:set_model()
	self.object:animate_spec("idle")
	if self.dump_presets then
		print(self.object:write_preset())
	end
end

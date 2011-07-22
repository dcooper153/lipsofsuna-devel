Views.Chargen = Class(Widgets.Scene)
Views.Chargen.class_name = "Views.Chargen"
Views.Chargen.list_races = {
	{"Aer - female", "aer"},
	{"Aer - male", "aermale"},
	{"Android - female", "android"},
	{"Android - male", "androidmale"},
	{"Devora - genderless", "devora"},
	{"Kraken - female", "kraken"},
	{"Wyrm - female", "wyrm"},
	{"Wyrm - male", "wyrmmale"}}

Views.Chargen.list_spawnpoints={{"Tutorial","lyra"},{"Lips Town","mentor"},{"Home (if you've made one)","home"}}

--- Creates a new chargen view.
-- @param clss Chargen class.
-- @return Chargen.
Views.Chargen.new = function(clss)
	-- Preview scene.
	local camera = Camera{far = 60.0, near = 0.3, mode = "first-person"}
	local self = Widgets.Scene.new(clss, {rows = 1, behind = true, fullscreen = true, camera = camera, spacings = {0,0}})
	self.offset = Vector(0, 1.8, -2)
	self.margins = {5,5,5,5}
	self.skills = Widgets.Skills{changed = function(widget, skill)
		skill.value = skill.cap
	end}
	self.object = Object{position = Vector(1, 1, 1), type = "character"}
	self.light = Light{ambient = {1.0,1.0,1.0,1.0}, diffuse = {1.0,1.0,1.0,1.0}, equation = {2,0.3,0.03}}
	self.timer = Timer{enabled = false, func = function(timer, secs) self:update(secs) end}
	self.list_hair_styles = {}
	self.list_eye_styles = {}
	self.list_skin_styles = {}
	-- Spawn Point Selector
	local spawnpoints = {}
	for k,v in ipairs(self.list_spawnpoints) do
		table.insert(spawnpoints, {v[1], function() end})
	end
	self.label_spawn = Widgets.Label{text="Spawn Point:"}
	self.combo_spawn = Widgets.ComboBox(spawnpoints)
	--self.combo_spawn.activated = function() self.list_spawnpoints[self.combo_spawn.value][2]() end
	-- Character name.
	self.label_name = Widgets.Label{text = "Name:"}
	self.entry_name = Widgets.Entry()
	
	-- Race selector.
	local races = {}
	for k,v in ipairs(self.list_races) do
		table.insert(races, {v[1], function() self:set_race(k) end})
	end
	self.label_race = Widgets.Label{text = "Race:"}
	self.label_race:set_request{width = 100}
	self.combo_race = Widgets.ComboBox(races)
	-- Eye style and color selectors.
	self.label_eye_style = Widgets.Label{text = "Eyes:"}
	self.combo_eye_style = Widgets.ComboBox()
	self.label_eye_color = Widgets.Label{text = "  Color:"}
	self.color_eye = Widgets.ColorSelector{pressed = function(widget, point)
		Widgets.ColorSelector.pressed(widget, point)
		self:update_model()
	end}
	-- Hair style and color selectors.
	self.label_hair_style = Widgets.Label{text = "Hair:"}
	self.combo_hair_style = Widgets.ComboBox()
	self.label_hair_color = Widgets.Label{text = "  Color:"}
	self.color_hair = Widgets.ColorSelector{pressed = function(widget, point)
		Widgets.ColorSelector.pressed(widget, point)
		self:update_model()
	end}
	-- Skin style and color selectors.
	self.label_skin_style = Widgets.Label{text = "Skin:"}
	self.combo_skin_style = Widgets.ComboBox()
	self.label_skin_color = Widgets.Label{text = "  Color:"}
	self.color_skin = Widgets.ColorSelector{pressed = function(widget, point)
		Widgets.ColorSelector.pressed(widget, point)
		self:update_model()
	end}
	-- Body style sliders.
	self.label_height = Widgets.Label{text = "Height:"}
	self.scroll_height = Widgets.Progress{min = 0, max = 1, value = 1,
		pressed = function(widget) self:set_height(widget:get_value_at(Program.cursor_position)) end}
	self.label_body = {}
	self.scroll_body = {}
	local body_slider_names = {
		"Arm muscularity", "Body width", "Bust size", "Hips width", "Leg muscularity",
			"Torso width", "Waist fatness", "Waist width"}
	local body_slider_inverse = {
		false, true, false, false, false, false, false, false}
	for k,v in ipairs(body_slider_names) do
		self.label_body[k] = Widgets.Label{text = v}
		self.scroll_body[k] = Widgets.Progress{min = 0, max = 1, value = 0, inverse = body_slider_inverse[k],
			pressed = function(widget) self:set_body(k, widget:get_value_at(Program.cursor_position)) end}
	end
	-- Face style sliders.
	self.label_face = {}
	self.scroll_face = {}
	local face_slider_names = {
		"Cheekbone size", "Cheek size", "Chin sharpness", "Chin size",
		"Eye inner", "Eye distance", "Eye outer", "Eye size",
		"Face roughness", "Jaw pronounced", "Jaw width", "Lips protruded",
		"Mouth width", "Nose pointedness", "Nose position"}
	local face_slider_inverse = {
		true, true, false, true, false, true, false, true,
		false, true, false, false, false, true, false}
	for k,v in ipairs(face_slider_names) do
		self.label_face[k] = Widgets.Label{text = v}
		self.scroll_face[k] = Widgets.Progress{min = 0, max = 1, value = 0, inverse = face_slider_inverse[k],
			pressed = function(widget) self:set_face(k, widget:get_value_at(Program.cursor_position)) end}
	end
	-- Apply and quit buttons.
	self.button_create = Widgets.Button{text = "Create", pressed = function() self:apply() end}
	self.button_create:set_request{height = 40}
	self.button_quit = Widgets.Button{text = "Quit", pressed = function() self:quit() end}
	-- Packing.
	self.group_hair = Widget{rows = 1, cols = 2, homogeneous = true}
	self.group_hair:set_child{row = 1, col = 1, widget = self.combo_hair_style}
	self.group_hair:set_child{row = 1, col = 2, widget = self.combo_hair_color}
	self.group_hair:set_expand{col = 1}
	self.group_hair:set_expand{col = 2}
	self.group_race = Widget{rows = 2, cols = 2, spacings = {0,2}}
	self.group_race:set_child{row = 1, col = 1, widget = self.label_name}
	self.group_race:set_child{row = 1, col = 2, widget = self.entry_name}
	self.group_race:set_child{row = 2, col = 1, widget = self.label_race}
	self.group_race:set_child{row = 2, col = 2, widget = self.combo_race}
	self.group_race:set_expand{col = 2}
	self.group_race1 = Widgets.Frame{rows = 2, cols = 1}
	self.group_race1:set_child{row = 1, col = 1, widget = self.group_race}
	self.group_race1:set_child{row = 2, col = 1, widget = self.skills}
	self.group_appearance = Widgets.Frame{rows = 7 + #body_slider_names + #face_slider_names, cols = 2, spacings = {0,2}}
	self.group_appearance:set_child{row = 1, col = 1, widget = self.label_eye_style}
	self.group_appearance:set_child{row = 1, col = 2, widget = self.combo_eye_style}
	self.group_appearance:set_child{row = 2, col = 1, widget = self.label_eye_color}
	self.group_appearance:set_child{row = 2, col = 2, widget = self.color_eye}
	self.group_appearance:set_child{row = 3, col = 1, widget = self.label_hair_style}
	self.group_appearance:set_child{row = 3, col = 2, widget = self.combo_hair_style}
	self.group_appearance:set_child{row = 4, col = 1, widget = self.label_hair_color}
	self.group_appearance:set_child{row = 4, col = 2, widget = self.color_hair}
	self.group_appearance:set_child{row = 5, col = 1, widget = self.label_skin_style}
	self.group_appearance:set_child{row = 5, col = 2, widget = self.combo_skin_style}
	self.group_appearance:set_child{row = 6, col = 1, widget = self.label_skin_color}
	self.group_appearance:set_child{row = 6, col = 2, widget = self.color_skin}
	self.group_appearance:set_child{row = 7, col = 1, widget = self.label_height}
	self.group_appearance:set_child{row = 7, col = 2, widget = self.scroll_height}
	local row = 8
	for i = 1,#body_slider_names do
		self.group_appearance:set_child{row = row, col = 1, widget = self.label_body[i]}
		self.group_appearance:set_child{row = row, col = 2, widget = self.scroll_body[i]}
		row = row + 1
	end
	for i = 1,#face_slider_names do
		self.group_appearance:set_child{row = row, col = 1, widget = self.label_face[i]}
		self.group_appearance:set_child{row = row, col = 2, widget = self.scroll_face[i]}
		row = row + 1
	end
	self.group_appearance:set_expand{col = 2}
	self.group_spawn = Widget{rows = 1, cols =2, margins={0,0,0,0}}
	self.group_spawn:set_child{row=1,col=1,widget=self.label_spawn}
	self.group_spawn:set_child{row=1,col=2,widget=self.combo_spawn}
	self.group_buttons = Widget{rows = 3, cols = 1, margins = {0,0,5,5}}
	self.group_buttons:set_child{row = 1, col = 1, widget = self.group_spawn}
	self.group_buttons:set_child{row = 2, col = 1, widget = self.button_create}
	self.group_buttons:set_child{row = 3, col = 1, widget = self.button_quit}
	self.group_buttons:set_expand{col = 1}
	self.group_left = Widget{cols = 1, spacings = {0,0}}
	self.group_left:append_row(Widgets.Frame{style = "title", text = "Character"})
	self.group_left:append_row(self.group_race1)
	self.group_left:append_row(self.group_buttons)
	self.group_left:set_expand{col = 1}
	self.group_right = Widget{cols = 1, spacings = {0,0}}
	self.group_right:append_row(Widgets.Frame{style = "title", text = "Apperance"})
	self.group_right:append_row(self.group_appearance)
	self.group_right:set_expand{col = 1}
	self:append_col(self.group_left)
	self:append_col(nil)
	self:append_col(self.group_right)
	self:set_expand{row = 1, col = 2}
	return self
end

Views.Chargen.apply = function(self)
	local scroll_value = function(scroll)
		return 255 * (scroll.inverse and (1 - scroll.value) or scroll.value)
	end
	local packet = Packet(packets.CHARACTER_CREATE,
		"string", self.entry_name.text,
		"string", self.list_races[self.combo_race.value][2],
		-- Skills.
		"uint8", self.skills:get_value("dexterity"),
		"uint8", self.skills:get_value("health"),
		"uint8", self.skills:get_value("intelligence"),
		"uint8", self.skills:get_value("perception"),
		"uint8", self.skills:get_value("strength"),
		"uint8", self.skills:get_value("willpower"),
		-- Body style.
		"uint8", scroll_value(self.scroll_height),
		"uint8", scroll_value(self.scroll_body[1]),
		"uint8", scroll_value(self.scroll_body[2]),
		"uint8", scroll_value(self.scroll_body[3]),
		"uint8", scroll_value(self.scroll_body[4]),
		"uint8", scroll_value(self.scroll_body[5]),
		"uint8", scroll_value(self.scroll_body[6]),
		"uint8", scroll_value(self.scroll_body[7]),
		"uint8", scroll_value(self.scroll_body[8]),
		-- Eye style.
		"string", "default",
		"uint8", 255 * self.color_eye.red,
		"uint8", 255 * self.color_eye.green,
		"uint8", 255 * self.color_eye.blue,
		-- Face style.
		"uint8", scroll_value(self.scroll_face[1]),
		"uint8", scroll_value(self.scroll_face[2]),
		"uint8", scroll_value(self.scroll_face[3]),
		"uint8", scroll_value(self.scroll_face[4]),
		"uint8", scroll_value(self.scroll_face[5]),
		"uint8", scroll_value(self.scroll_face[6]),
		"uint8", scroll_value(self.scroll_face[7]),
		"uint8", scroll_value(self.scroll_face[8]),
		"uint8", scroll_value(self.scroll_face[9]),
		"uint8", scroll_value(self.scroll_face[10]),
		"uint8", scroll_value(self.scroll_face[11]),
		"uint8", scroll_value(self.scroll_face[12]),
		"uint8", scroll_value(self.scroll_face[13]),
		"uint8", scroll_value(self.scroll_face[14]),
		"uint8", scroll_value(self.scroll_face[15]),
		-- Hair style.
		"string", self.list_hair_styles[self.combo_hair_style.value][2],
		"uint8", 255 * self.color_hair.red,
		"uint8", 255 * self.color_hair.green,
		"uint8", 255 * self.color_hair.blue,
		-- Skin style.
		"string", "default",
		"uint8", 255 * self.color_skin.red,
		"uint8", 255 * self.color_skin.green,
		"uint8", 255 * self.color_skin.blue,
		--Spawnpoint
		"string", self.list_spawnpoints[self.combo_spawn.value][2])
	Network:send{packet = packet}
end

Views.Chargen.close = function(self)
	self.floating = false
	self.object.realized = false
	self.timer.enabled = false
	self.light.enabled = false
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
	self.timer.enabled = true
	self.light.enabled = true
	self:random()
	self:update(0.0)
	self.camera:warp()
	Sound:switch_music_track("char")
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
	elseif args.type == "mousemotion" then
		if self.dragging then
			local y = self.offset.y + args.dy / 300
			self.offset.y = math.min(math.max(y, 1), 2)
			self:rotate(math.pi * args.dx / 300)
		end
	end
end

Views.Chargen.quit = function(self)
	Program.quit = true
end

--- Randomizes the character.
-- @param self Chargen.
Views.Chargen.random = function(self)
	self:set_race(math.random(1, #self.list_races))
	self.update_needed = true
end

--- Rotates the character.
-- @param self Chargen.
-- @param rad Radians.
Views.Chargen.rotate = function(self, rad)
	local rot = Quaternion{axis = Vector(0, 1, 0), angle = rad}
	self.object.rotation = self.object.rotation * rot
end

Views.Chargen.set_body = function(self, slider, value)
	self.scroll_body[slider].value = value
	self.update_needed = true
end

Views.Chargen.set_eye_style = function(self, index)
	self.combo_eye_style.value = index
	self.combo_eye_style.text = self.list_eye_styles[index][1]
	self.eye_style = self.list_eye_styles[index][2]
	self.update_needed = true
end

Views.Chargen.set_face = function(self, slider, value)
	self.scroll_face[slider].value = value
	self.update_needed = true
end

Views.Chargen.set_eye_color = function(self, r, g, b)
	self.color_eye.red = r
	self.color_eye.green = g
	self.color_eye.blue = b
	self.update_needed = true
end

Views.Chargen.set_hair_style = function(self, index)
	self.combo_hair_style.value = index
	self.combo_hair_style.text = self.list_hair_styles[index][1]
	self.hair_style = self.list_hair_styles[index][2]
	self.update_needed = true
end

Views.Chargen.set_hair_color = function(self, r, g, b)
	self.color_hair.red = r
	self.color_hair.green = g
	self.color_hair.blue = b
	self.update_needed = true
end

Views.Chargen.set_height = function(self, value)
	self.scroll_height.value = value
	self.update_needed = true
end

Views.Chargen.get_race = function(self)
	return Species:find{name = self.list_races[self.combo_race.value][2]}
end

Views.Chargen.set_race = function(self, index)
	-- Set the race selection.
	self.combo_race.value = index
	self.combo_race.text = self.list_races[index][1]
	self.race = self.list_races[index][2]
	local spec = self:get_race()
	-- Rebuild the eye style list.
	self.list_eye_styles = {}
	self.combo_eye_style:clear()
	for k,v in ipairs(spec.eye_styles) do
		self.list_eye_styles[k] = v
		self.combo_eye_style:append{text = v[1], pressed = function() self:set_eye_style(k) end}
	end
	-- Rebuild the hair style list.
	self.list_hair_styles = {}
	self.combo_hair_style:clear()
	for k,v in ipairs(spec.hair_styles) do
		self.list_hair_styles[k] = v
		self.combo_hair_style:append{text = v[1], pressed = function() self:set_hair_style(k) end}
	end
	-- Rebuild the skin style list.
	self.list_skin_styles = {}
	self.combo_skin_style:clear()
	for k,v in ipairs(spec.skin_styles) do
		self.list_skin_styles[k] = v
		self.combo_skin_style:append{text = v[1], pressed = function() self:set_skin_style(k) end}
	end
	-- Reconfigure the body proportion sliders.
	self:set_height(spec.body_scale[1] + math.random() * (spec.body_scale[2] - spec.body_scale[1]))
	for i = 1,#self.scroll_body do
		self.scroll_body[i].value = math.random()
	end
	-- Reconfigure the face sliders.
	for i = 1,#self.scroll_face do
		self.scroll_face[i].value = math.random()
	end
	-- Randomize the affected fields.
	self:set_eye_style(math.random(1, #self.list_eye_styles))
	self:set_eye_color(math.random(), math.random(), math.random())
	self:set_hair_style(math.random(1, #self.list_hair_styles))
	self:set_hair_color(math.random(), math.random(), math.random())
	self:set_skin_style(math.random(1, #self.list_skin_styles))
	self.update_needed = true
	-- Randomize the name.
	self.entry_name.text = Names:random{race = self.list_races[self.combo_race.value][2], gender = "female"}
	-- Reset skills.
	self.skills:set_species(spec)
end

Views.Chargen.set_skin_style = function(self, index)
	self.combo_skin_style.value = index
	self.combo_skin_style.text = self.list_skin_styles[index][1]
	self.skin_style = self.list_skin_styles[index][2]
	self.update_needed = true
end

Views.Chargen.set_skin_color = function(self, r, g, b)
	self.color_skin.red = r
	self.color_skin.green = g
	self.color_skin.blue = b
	self.update_needed = true
end

Views.Chargen.update = function(self, secs)
	-- Update model.
	if self.update_needed then
		self.update_needed = nil
		self:update_model()
	end
	self.object:refresh()
	-- Update light.
	self.light.position = self.object.position + self.object.rotation * Vector(0, 2, -5)
	-- Update camera.
	self.camera.target_position = self.object.position + self.offset
	self.camera.target_rotation = Quaternion{axis = Vector(0, 1, 0), angle = math.pi}
	self.camera.viewport = {self.x, self.y, self.width, self.height}
	self.camera:update(secs)
end

Views.Chargen.update_model = function(self)
	local spec = Species:find{name = self.race .. "-player"}
	local items = {}
	for k,v in pairs(spec.inventory_items) do
		table.insert(items, k)
	end
	local body = {}
	for k,v in pairs(self.scroll_body) do
		if v.inverse then
			body[k] = 1 - v.value
		else
			body[k] = v.value
		end
	end
	local face = {}
	for k,v in pairs(self.scroll_face) do
		if v.inverse then
			face[k] = 1 - v.value
		else
			face[k] = v.value
		end
	end
	self.object.spec = spec
	self.object.body_scale = self.scroll_height.value
	self.object.body_style = body
	self.object.equipment = items
	self.object.eye_color = {self.color_eye.red, self.color_eye.green, self.color_eye.blue}
	self.object.eye_style = nil
	self.object.face_style = face
	self.object.hair_color = {self.color_hair.red, self.color_hair.green, self.color_hair.blue}
	self.object.hair_style = self.hair_style
	self.object.skin_color = {self.color_skin.red, self.color_skin.green, self.color_skin.blue}
	self.object.skin_style = nil
	self.object:update_model()
	self.object:animate{animation = "idle", channel = 1, permanent = true}
	self.object:update_animations{secs = 1}
	self.object:deform_mesh()
end

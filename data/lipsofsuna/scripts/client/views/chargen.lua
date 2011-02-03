require "client/views/skills"

Views.Chargen = Class(Widgets.Scene)
Views.Chargen.mode = "chargen"
Views.Chargen.list_races = {
	{"Aer", "aer"},
	{"Android", "android"},
	{"Devora", "devora"},
	{"Kraken", "kraken"},
	{"Wyrm", "wyrm"}}

--- Creates a new chargen view.
-- @param clss Chargen class.
-- @return Chargen.
Views.Chargen.new = function(clss)
	-- Preview scene.
	local camera = Camera{far = 60.0, near = 0.3, mode = "first-person"}
	local self = Widgets.Scene.new(clss, {cols = 1, behind = true, fullscreen = true, camera = camera})
	self.margins = {5,5,5,5}
	self.skills = Views.Skills()
	self.object = Object{position = Vector(1, 1, 1), type = "character"}
	self.light = Light{ambient = {1.0,1.0,1.0,1.0}, diffuse={1.0,1.0,1.0,1.0}, equation={2,0.3,0.03}}
	self.timer = Timer{enabled = false, func = function(timer, secs) self:update(secs) end}
	self.list_genders = {}
	self.list_hair_styles = {}
	self.list_eye_styles = {}
	self.list_skin_styles = {}
	-- Character name.
	self.label_name = Widgets.Label{text = "Name:"}
	self.entry_name = Widgets.Entry()
	-- Race and gender selectors.
	local races = {}
	for k,v in ipairs(self.list_races) do
		table.insert(races, {v[1], function() self:set_race(k) end})
	end
	self.label_race = Widgets.Label{text = "Race:"}
	self.combo_race = Widgets.ComboBox(races)
	self.label_gender = Widgets.Label{text = "Gender:"}
	self.combo_gender = Widgets.ComboBox()
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
	-- Body proportion sliders.
	self.label_height = Widgets.Label{text = "Height:"}
	self.scroll_height = Widgets.Progress{min = 0, max = 1, value = 1,
		pressed = function(widget) self:set_height(widget:get_value_at(Client.cursor_pos)) end}
	self.label_nose_scale = Widgets.Label{text = "Nose:"}
	self.scroll_nose_scale = Widgets.Progress{min = 0, max = 1, value = 1,
		pressed = function(widget) self:set_nose_scale(widget:get_value_at(Client.cursor_pos)) end}
	self.label_bust_scale = Widgets.Label{text = "Bust:"}
	self.scroll_bust_scale = Widgets.Progress{min = 0, max = 1, value = 1,
		pressed = function(widget) self:set_bust_scale(widget:get_value_at(Client.cursor_pos)) end}
	-- Apply and quit buttons.
	self.button_create = Widgets.Button{text = "Create", pressed = function() self:apply() end}
	self.button_quit = Widgets.Button{text = "Quit", pressed = function() self:quit() end}
	-- Packing.
	self.group_hair = Widget{rows = 1, cols = 2, homogeneous = true}
	self.group_hair:set_child{row = 1, col = 1, widget = self.combo_hair_style}
	self.group_hair:set_child{row = 1, col = 2, widget = self.combo_hair_color}
	self.group_hair:set_expand{col = 1}
	self.group_hair:set_expand{col = 2}
	self.group_race = Widget{rows = 12, cols = 2, homogeneous = true}
	self.group_race:set_child{row = 1, col = 1, widget = self.label_name}
	self.group_race:set_child{row = 1, col = 2, widget = self.entry_name}
	self.group_race:set_child{row = 2, col = 1, widget = self.label_race}
	self.group_race:set_child{row = 2, col = 2, widget = self.combo_race}
	self.group_race:set_child{row = 3, col = 1, widget = self.label_gender}
	self.group_race:set_child{row = 3, col = 2, widget = self.combo_gender}
	self.group_race:set_child{row = 4, col = 1, widget = self.label_eye_style}
	self.group_race:set_child{row = 4, col = 2, widget = self.combo_eye_style}
	self.group_race:set_child{row = 5, col = 1, widget = self.label_eye_color}
	self.group_race:set_child{row = 5, col = 2, widget = self.color_eye}
	self.group_race:set_child{row = 6, col = 1, widget = self.label_hair_style}
	self.group_race:set_child{row = 6, col = 2, widget = self.combo_hair_style}
	self.group_race:set_child{row = 7, col = 1, widget = self.label_hair_color}
	self.group_race:set_child{row = 7, col = 2, widget = self.color_hair}
	self.group_race:set_child{row = 8, col = 1, widget = self.label_skin_style}
	self.group_race:set_child{row = 8, col = 2, widget = self.combo_skin_style}
	self.group_race:set_child{row = 9, col = 1, widget = self.label_skin_color}
	self.group_race:set_child{row = 9, col = 2, widget = self.color_skin}
	self.group_race:set_child{row = 10, col = 1, widget = self.label_height}
	self.group_race:set_child{row = 10, col = 2, widget = self.scroll_height}
	self.group_race:set_child{row = 11, col = 1, widget = self.label_nose_scale}
	self.group_race:set_child{row = 11, col = 2, widget = self.scroll_nose_scale}
	self.group_race:set_child{row = 12, col = 1, widget = self.label_bust_scale}
	self.group_race:set_child{row = 12, col = 2, widget = self.scroll_bust_scale}
	self.group_race:set_expand{col = 2}
	self.group_race:set_request{width = 300}
	self.group_left = Widget{cols = 1}
	self.group_left:append_row(Widgets.Label{text = "Create character", font = "medium"})
	self.group_left:append_row(self.group_race)
	self.group_left:append_row(self.button_create)
	self.group_left:append_row(self.button_quit)
	self.group_left:set_expand{col = 1}
	self.group_left:set_request{width = 300}
	self:append_row(self.group_left)
	self:append_row(self.skills)
	self:set_expand{row = 2}
	return self
end

Views.Chargen.apply = function(self)
	local packet = Packet(packets.CHARACTER_CREATE,
		"string", self.entry_name.text,
		"string", self.list_races[self.combo_race.value][2],
		"string", self.list_genders[self.combo_gender.value][2],
		"uint8", self.skills:get_value("dexterity"),
		"uint8", self.skills:get_value("health"),
		"uint8", self.skills:get_value("intelligence"),
		"uint8", self.skills:get_value("perception"),
		"uint8", self.skills:get_value("strength"),
		"uint8", self.skills:get_value("willpower"),
		"float", self.scroll_height.value,
		"float", self.scroll_nose_scale.value,
		"float", self.scroll_bust_scale.value,
		"string", "default",
		"uint8", 255 * self.color_eye.red,
		"uint8", 255 * self.color_eye.green,
		"uint8", 255 * self.color_eye.blue,
		"string", self.list_hair_styles[self.combo_hair_style.value][2],
		"uint8", 255 * self.color_hair.red,
		"uint8", 255 * self.color_hair.green,
		"uint8", 255 * self.color_hair.blue,
		"string", "default",
		"uint8", 255 * self.color_skin.red,
		"uint8", 255 * self.color_skin.green,
		"uint8", 255 * self.color_skin.blue)
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
Views.Chargen.enter = function(self)
	self.floating = true
	self.object.realized = true
	self.timer.enabled = true
	self.light.enabled = true
	self:random()
	self:update(0.0)
	self.camera:warp()
end

Views.Chargen.quit = function(self)
	Program.quit = true
end

--- Randomizes the character
-- @param self Chargen.
Views.Chargen.random = function(self)
	self:set_race(math.random(1, #self.list_races))
	self:update_model()
end

Views.Chargen.set_gender = function(self, index)
	self.combo_gender.value = index
	self.combo_gender.text = self.list_genders[index][1]
	self.gender = self.list_genders[index][2]
	self:update_model()
end

Views.Chargen.set_bust_scale = function(self, value)
	self.scroll_bust_scale.value = value
	self:update_model()
end

Views.Chargen.set_eye_style = function(self, index)
	self.combo_eye_style.value = index
	self.combo_eye_style.text = self.list_eye_styles[index][1]
	self.eye_style = self.list_eye_styles[index][2]
	self:update_model()
end

Views.Chargen.set_eye_color = function(self, r, g, b)
	self.color_eye.red = r
	self.color_eye.green = g
	self.color_eye.blue = b
	self:update_model()
end

Views.Chargen.set_hair_style = function(self, index)
	self.combo_hair_style.value = index
	self.combo_hair_style.text = self.list_hair_styles[index][1]
	self.hair_style = self.list_hair_styles[index][2]
	self:update_model()
end

Views.Chargen.set_hair_color = function(self, r, g, b)
	self.color_hair.red = r
	self.color_hair.green = g
	self.color_hair.blue = b
	self:update_model()
end

Views.Chargen.set_height = function(self, value)
	self.scroll_height.value = value
	self:update_model()
end

Views.Chargen.set_nose_scale = function(self, value)
	self.scroll_nose_scale.value = value
	self:update_model()
end

Views.Chargen.set_race = function(self, index)
	-- Set the race selection.
	self.combo_race.value = index
	self.combo_race.text = self.list_races[index][1]
	self.race = self.list_races[index][2]
	local spec = Species:find{name = self.list_races[self.combo_race.value][2]}
	-- Rebuild the gender list.
	self.list_genders = {}
	self.combo_gender:clear()
	for k,v in ipairs(spec.genders) do
		self.list_genders[k] = v
		self.combo_gender:append{text = v[1], pressed = function() self:set_gender(k) end}
	end
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
	self.scroll_height.value = 1
	self.scroll_height.min = spec.body_scale[1]
	self.scroll_height.max = spec.body_scale[2]
	self.scroll_nose_scale.value = 1
	self.scroll_nose_scale.min = spec.nose_scale[1]
	self.scroll_nose_scale.max = spec.nose_scale[2]
	self.scroll_bust_scale.value = 1
	self.scroll_bust_scale.min = spec.bust_scale[1]
	self.scroll_bust_scale.max = spec.bust_scale[2]
	-- Randomize the affected fields.
	self:set_gender(1)--math.random(1, #self.list_genders))
	self:set_eye_style(math.random(1, #self.list_eye_styles))
	self:set_eye_color(math.random(), math.random(), math.random())
	self:set_hair_style(math.random(1, #self.list_hair_styles))
	self:set_hair_color(math.random(), math.random(), math.random())
	self:set_skin_style(math.random(1, #self.list_skin_styles))
	self:set_height(spec.body_scale[1] + math.random() * (spec.body_scale[2] - spec.body_scale[1]))
	self:set_bust_scale(spec.bust_scale[1] + math.random() * (spec.bust_scale[2] - spec.bust_scale[1]))
	self:set_nose_scale(spec.nose_scale[1] + math.random() * (spec.nose_scale[2] - spec.nose_scale[1]))
	self:update_model()
	-- Randomize the name.
	self.entry_name.text = Names:random{race = self.list_races[self.combo_race.value][2], gender = "female"}
	-- Reset skills.
	self.skills:set_species(spec)
end

Views.Chargen.set_skin_style = function(self, index)
	self.combo_skin_style.value = index
	self.combo_skin_style.text = self.list_skin_styles[index][1]
	self.skin_style = self.list_skin_styles[index][2]
	self:update_model()
end

Views.Chargen.set_skin_color = function(self, r, g, b)
	self.color_skin.red = r
	self.color_skin.green = g
	self.color_skin.blue = b
	self:update_model()
end

Views.Chargen.update = function(self, secs)
	-- Update model.
	local rot = Quaternion{axis = Vector(0, 1, 0), angle = math.pi * 0.1 * secs}
	self.object.rotation = self.object.rotation * rot
	self.object:refresh()
	-- Update light.
	self.light.position = self.object.position + self.object.rotation * Vector(0, 2, -5)
	-- Update camera.
	self.camera.target_position = self.object.position + Vector(0, 1.8, -2)
	self.camera.target_rotation = Quaternion{axis = Vector(0, 1, 0), angle = math.pi}
	self.camera.viewport = {self.x, self.y, self.width, self.height}
	self.camera:update(secs)
end

Views.Chargen.update_model = function(self)
	self.object:create_character_model{
		body_scale = self.scroll_height.value,
		bust_scale = self.scroll_bust_scale.value,
		equipment = {"dress"}, -- TODO
		eye_color = {self.color_eye.red, self.color_eye.green, self.color_eye.blue},
		gender = self.gender,
		hair_color = {self.color_hair.red, self.color_hair.green, self.color_hair.blue},
		hair_style = self.hair_style,
		nose_scale = self.scroll_nose_scale.value,
		race = self.race,
		skin_color = {self.color_skin.red, self.color_skin.green, self.color_skin.blue}}
	self.object:animate{animation = "walk", channel = 1, permanent = true}
	self.object:update_animations{secs = 1}
	self.object:deform_mesh()
end

------------------------------------------------------------------------------

Views.Chargen.inst = Views.Chargen()

Protocol:add_handler{type = "CHARACTER_ACCEPT", func = function(event)
	Gui:set_mode("menu")
end}
Protocol:add_handler{type = "CHARACTER_CREATE", func = function(event)
	Gui:set_mode("chargen")
end}
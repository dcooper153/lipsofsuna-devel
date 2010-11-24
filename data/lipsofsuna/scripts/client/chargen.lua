Chargen = Class()
Chargen.visible = false
Chargen.list_races = {
	{"Aer", "aer"},
	{"Android", "android"},
	{"Devora", "devora"},
	{"Kraken", "kraken"},
	{"Wyrm", "wyrm"}}
Chargen.list_genders = {
	{"Female", "female"},
	{"Male", "male"}}
Chargen.list_hair_styles = {
	{"Bald", ""},
	{"Pigtails", "hair1"},
	{"Long", "hair2"}}
Chargen.list_eye_styles = {
	{"Default", ""}} -- TODO
Chargen.list_skin_styles = {
	{"Default", ""}} -- TODO

--- Initializes the character generator.
-- @param clss Chargen class.
Chargen.init = function(clss)
	-- Race and hair selectors.
	local races = {}
	local genders = {}
	local hair_styles = {}
	local hair_colors = {}
	for k,v in ipairs(clss.list_races) do
		table.insert(races, {v[1], function() Chargen:set_race(k) end})
	end
	for k,v in ipairs(clss.list_genders) do
		table.insert(genders, {v[1], function() Chargen:set_gender(k) end})
	end
	for k,v in ipairs(clss.list_hair_styles) do
		table.insert(hair_styles, {v[1], function() Chargen:set_hair_style(k) end})
	end
	clss.label_race = Widgets.Label{text = "Race:"}
	clss.combo_race = Widgets.ComboBox(races)
	clss.label_gender = Widgets.Label{text = "Gender:"}
	clss.combo_gender = Widgets.ComboBox(genders)
	-- Eye style and color selectors.
	clss.label_eye_style = Widgets.Label{text = "Eyes:"}
	clss.combo_eye_style = Widgets.ComboBox(eye_styles)
	clss.label_eye_color = Widgets.Label{text = "  Color:"}
	clss.color_eye = Widgets.ColorSelector{pressed = function(self, point)
		Widgets.ColorSelector.pressed(self, point)
		clss:update_model()
	end}
	-- Hair style and color selectors.
	clss.label_hair_style = Widgets.Label{text = "Hair:"}
	clss.combo_hair_style = Widgets.ComboBox(hair_styles)
	clss.label_hair_color = Widgets.Label{text = "  Color:"}
	clss.color_hair = Widgets.ColorSelector{pressed = function(self, point)
		Widgets.ColorSelector.pressed(self, point)
		clss:update_model()
	end}
	-- Skin style and color selectors.
	clss.label_skin_style = Widgets.Label{text = "Skin:"}
	clss.combo_skin_style = Widgets.ComboBox(skin_styles)
	clss.label_skin_color = Widgets.Label{text = "  Color:"}
	clss.color_skin = Widgets.ColorSelector{pressed = function(self, point)
		Widgets.ColorSelector.pressed(self, point)
		clss:update_model()
	end}
	-- Body proportion sliders.
	clss.label_height = Widgets.Label{text = "Height:"}
	clss.scroll_height = Widgets.Progress{min = 0.9, max = 1.05, value = 1,
		pressed = function(self) clss:set_height(self:get_value_at(Client.cursor_pos)) end}
	clss.label_nose_scale = Widgets.Label{text = "Nose:"}
	clss.scroll_nose_scale = Widgets.Progress{min = 0.5, max = 2.0, value = 1,
		pressed = function(self) clss:set_nose_scale(self:get_value_at(Client.cursor_pos)) end}
	clss.label_bust_scale = Widgets.Label{text = "Bust:"}
	clss.scroll_bust_scale = Widgets.Progress{min = 0.3, max = 1.3, value = 1,
		pressed = function(self) clss:set_bust_scale(self:get_value_at(Client.cursor_pos)) end}
	-- Preview widget.
	clss.scene = Scene()
	clss.object = Object{position = Vector(1, 1, 1), type = "character"}
	clss.light = Light{ambient = {1.0,1.0,1.0,1.0}, diffuse={1.0,1.0,1.0,1.0}, equation={2,0.3,0.03}}
	clss.camera = Camera{far = 60.0, near = 0.3, mode = "first-person"}
	clss.timer = Timer{enabled = false, func = function(self, secs) clss:update(secs) end}
	clss.preview = Widgets.Scene{cols = 1, behind = true, fullscreen = true, camera = clss.camera}
	clss.preview.margins = {5,5,5,5}
	-- Apply and quit buttons.
	clss.button_create = Widgets.Button{text = "Create", pressed = function() clss:apply() end}
	clss.button_quit = Widgets.Button{text = "Quit", pressed = function() clss:quit() end}
	-- Packing.
	clss.group_hair = Widget{rows = 1, cols = 2, homogeneous = true}
	clss.group_hair:set_child{row = 1, col = 1, widget = clss.combo_hair_style}
	clss.group_hair:set_child{row = 1, col = 2, widget = clss.combo_hair_color}
	clss.group_hair:set_expand{col = 1}
	clss.group_hair:set_expand{col = 2}
	clss.group_race = Widget{rows = 11, cols = 2, homogeneous = true}
	clss.group_race:set_child{row = 1, col = 1, widget = clss.label_race}
	clss.group_race:set_child{row = 1, col = 2, widget = clss.combo_race}
	clss.group_race:set_child{row = 2, col = 1, widget = clss.label_gender}
	clss.group_race:set_child{row = 2, col = 2, widget = clss.combo_gender}
	clss.group_race:set_child{row = 3, col = 1, widget = clss.label_eye_style}
	clss.group_race:set_child{row = 3, col = 2, widget = clss.combo_eye_style}
	clss.group_race:set_child{row = 4, col = 1, widget = clss.label_eye_color}
	clss.group_race:set_child{row = 4, col = 2, widget = clss.color_eye}
	clss.group_race:set_child{row = 5, col = 1, widget = clss.label_hair_style}
	clss.group_race:set_child{row = 5, col = 2, widget = clss.combo_hair_style}
	clss.group_race:set_child{row = 6, col = 1, widget = clss.label_hair_color}
	clss.group_race:set_child{row = 6, col = 2, widget = clss.color_hair}
	clss.group_race:set_child{row = 7, col = 1, widget = clss.label_skin_style}
	clss.group_race:set_child{row = 7, col = 2, widget = clss.combo_skin_style}
	clss.group_race:set_child{row = 8, col = 1, widget = clss.label_skin_color}
	clss.group_race:set_child{row = 8, col = 2, widget = clss.color_skin}
	clss.group_race:set_child{row = 9, col = 1, widget = clss.label_height}
	clss.group_race:set_child{row = 9, col = 2, widget = clss.scroll_height}
	clss.group_race:set_child{row = 10, col = 1, widget = clss.label_nose_scale}
	clss.group_race:set_child{row = 10, col = 2, widget = clss.scroll_nose_scale}
	clss.group_race:set_child{row = 11, col = 1, widget = clss.label_bust_scale}
	clss.group_race:set_child{row = 11, col = 2, widget = clss.scroll_bust_scale}
	clss.group_race:set_expand{col = 2}
	clss.group_race:set_request{width = 300}
	clss.group_left = Widget{cols = 1}
	clss.group_left:append_row(Widgets.Label{text = "Create character", font = "medium"})
	clss.group_left:append_row(clss.group_race)
	clss.group_left:append_row(clss.button_create)
	clss.group_left:append_row(clss.button_quit)
	clss.group_left:set_expand{col = 1}
	clss.group_left:set_request{width = 300}
	clss.group = clss.preview
	clss.group:append_row(clss.group_left)
	clss.group:set_expand{row = 1}
end

Chargen.apply = function(clss)
	local packet = Packet(packets.CHARACTER_CREATE,
		"string", clss.list_races[clss.combo_race.value][2],
		"string", clss.list_genders[clss.combo_gender.value][2],
		"float", clss.scroll_height.value,
		"float", clss.scroll_nose_scale.value,
		"float", clss.scroll_bust_scale.value,
		"string", "default",
		"uint8", 255 * clss.color_eye.red,
		"uint8", 255 * clss.color_eye.green,
		"uint8", 255 * clss.color_eye.blue,
		"string", clss.list_hair_styles[clss.combo_hair_style.value][2],
		"uint8", 255 * clss.color_hair.red,
		"uint8", 255 * clss.color_hair.green,
		"uint8", 255 * clss.color_hair.blue,
		"string", "default",
		"uint8", 255 * clss.color_skin.red,
		"uint8", 255 * clss.color_skin.green,
		"uint8", 255 * clss.color_skin.blue)
	Network:send{packet = packet}
end

--- Executes the character generator.
-- @param clss Chargen class.
Chargen.execute = function(clss)
	Gui:free()
	Client.moving = false
	Sound.music_fading = 5.0
	Sound.music_volume = 0.2
	Sound.music = "fairytale10"
	clss.group.floating = true
	clss.object.realized = true
	clss.timer.enabled = true
	clss.light.enabled = true
	clss:random()
end

Chargen.free = function(clss)
	clss.group.floating = false
	clss.object.realized = false
	clss.timer.enabled = false
	clss.light.enabled = false
end

Chargen.quit = function(clss)
	Program.quit = true
end

--- Randomizes the character
-- @param clss Chargen class.
Chargen.random = function(clss)
	clss:set_race(1)
	clss:set_gender(1)
	clss:set_hair_style(2)
	clss:set_hair_color(1, 1, 1)
	clss:update_model()
end

Chargen.set_gender = function(clss, index)
	clss.combo_gender.value = index
	clss.combo_gender.text = clss.list_genders[index][1]
	clss.gender = clss.list_genders[index][2]
	clss:update_model()
end

Chargen.set_bust_scale = function(clss, value)
	clss.scroll_bust_scale.value = value
	clss:update_model()
end

Chargen.set_eye_style = function(clss, index)
	clss.combo_eye_style.value = index
	clss.combo_eye_style.text = clss.list_eye_styles[index][1]
	clss.eye_style = clss.list_eye_styles[index][2]
	clss:update_model()
end

Chargen.set_eye_color = function(clss, r, g, b)
	clss.color_eye.red = r
	clss.color_eye.green = g
	clss.color_eye.blue = b
	clss:update_model()
end

Chargen.set_hair_style = function(clss, index)
	clss.combo_hair_style.value = index
	clss.combo_hair_style.text = clss.list_hair_styles[index][1]
	clss.hair_style = clss.list_hair_styles[index][2]
	clss:update_model()
end

Chargen.set_hair_color = function(clss, r, g, b)
	clss.color_hair.red = r
	clss.color_hair.green = g
	clss.color_hair.blue = b
	clss:update_model()
end

Chargen.set_height = function(clss, value)
	clss.scroll_height.value = value
	clss:update_model()
end

Chargen.set_nose_scale = function(clss, value)
	clss.scroll_nose_scale.value = value
	clss:update_model()
end

Chargen.set_race = function(clss, index)
	clss.combo_race.value = index
	clss.combo_race.text = clss.list_races[index][1]
	clss.race = clss.list_races[index][2]
	clss:update_model()
end

Chargen.set_skin_style = function(clss, index)
	clss.combo_skin_style.value = index
	clss.combo_skin_style.text = clss.list_skin_styles[index][1]
	clss.skin_style = clss.list_skin_styles[index][2]
	clss:update_model()
end

Chargen.set_skin_color = function(clss, r, g, b)
	clss.color_skin.red = r
	clss.color_skin.green = g
	clss.color_skin.blue = b
	clss:update_model()
end

Chargen.update = function(clss, secs)
	-- Update model.
	local rot = Quaternion{axis = Vector(0, 1, 0), angle = math.pi * 0.1 * secs}
	clss.object.rotation = clss.object.rotation * rot
	clss.object:refresh()
	-- Update light.
	clss.light.position = clss.object.position + clss.object.rotation * Vector(0, 2, -5)
	-- Update camera.
	clss.camera.target_position = clss.object.position + Vector(0, 1.8, -2)
	clss.camera.target_rotation = Quaternion{axis = Vector(0, 1, 0), angle = math.pi}
	clss.camera.viewport = {clss.preview.x, clss.preview.y, clss.preview.width, clss.preview.height}
	clss.camera:update(secs)
end

Chargen.update_model = function(clss)
	clss.object:create_character_model{
		body_scale = clss.scroll_height.value,
		bust_scale = clss.scroll_bust_scale.value,
		equipment = {"dress"}, -- TODO
		eye_color = {clss.color_eye.red, clss.color_eye.green, clss.color_eye.blue},
		gender = clss.gender,
		hair_color = {clss.color_hair.red, clss.color_hair.green, clss.color_hair.blue},
		hair_style = clss.hair_style,
		nose_scale = clss.scroll_nose_scale.value,
		race = clss.race,
		skin_color = {clss.color_skin.red, clss.color_skin.green, clss.color_skin.blue}}
	clss.object:animate{animation = "walk", channel = 1, permanent = true}
	clss.object:update_animations{secs = 1}
	clss.object:deform_mesh()
end

------------------------------------------------------------------------------

Chargen:init()
Protocol:add_handler{type = "CHARACTER_ACCEPT", func = function(event)
	Chargen:free()
	Gui:init()
	Sound.music = "fairytale2"
	Sound.music_volume = 0.1
	Program:unload_world()
end}
Protocol:add_handler{type = "CHARACTER_CREATE", func = function(event)
	Chargen:execute()
end}


Ui:add_state{
	state = "chargen",
	root = "chargen",
	init = function()
		Sound:switch_music_track("char")
		Client.camera = Client.views.chargen.camera
	end,
	update = function(secs)
		Client.views.chargen:update(secs)
		Client:update_camera()
	end}

Ui:add_widget{
	state = "chargen",
	widget = function()
		local widget = Widgets.Uientry("Name", function(w)
			Client.views.chargen:set_name(w.value)
		end)
		widget.value = Client.views.chargen:get_name()
		return widget
	end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Race", "chargen/race") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Preset", "chargen/presets") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Body", "chargen/body") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Face", "chargen/face") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Misc", "chargen/misc") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uibutton("Create", function()
			Client.views.chargen:apply()
		end)
	end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Quit", "quit") end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/race",
	init = function()
		-- Create the race radio buttons.
		local widgets = {}
		for k,v in ipairs(Client.views.chargen.list_races) do
			local widget = Widgets.Uiradio(v[1], "race", function(w)
				Client.views.chargen:set_race(w.race)
			end)
			widget.race = v[2]
			table.insert(widgets, widget)
		end
		return widgets
	end,
	update = function(secs)
		Client.views.chargen:update(secs)
	end}

Ui:add_widget{
	state = "chargen/race",
	widget = function() return Widgets.Uibutton("Back", function() Ui:pop_state() end) end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/presets",
	init = function()
		-- Create the preset radio buttons.
		local widgets = {}
		for k,v in ipairs(Client.views.chargen.list_presets) do
			local widget = Widgets.Uiradio(v.name, "preset", function(w)
				Client.views.chargen:set_preset(w.preset)
			end)
			widget.preset = v
			table.insert(widgets, widget)
		end
		return widgets
	end,
	update = function(secs)
		Client.views.chargen:update(secs)
	end}

Ui:add_widget{
	state = "chargen/presets",
	widget = function() return Widgets.Uibutton("Back", function() Ui:pop_state() end) end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/body",
	init = function()
		local sliders = {
			{"Arm muscularity", false},
			{"Body width", true},
			{"Bust size", false},
			{"Hips width", false},
			{"Leg muscularity", false},
			{"Torso width", false},
			{"Waist fatness", false},
			{"Waist width", false}}
		-- Create the body proportion sliders.
		local widgets = {}
		for k,v in ipairs(sliders) do
			local value = Client.views.chargen:get_body(k)
			if v[2] then value = 1 - value end
			local widget = Widgets.Uiscrollfloat(v[1], 0, 1, value, function(w)
				Client.views.chargen:set_body(k, w.inverse and 1 - w.value or w.value)
			end)
			widget.inverse = v[2]
			table.insert(widgets, widget)
		end
		-- Create the height adjustment widget.
		local widget = Widgets.Uiscrollfloat("Height", 0, 1, 0, function(w)
			Client.views.chargen:set_height(w.value)
		end)
		table.insert(widgets, widget)
		return widgets
	end,
	update = function(secs)
		Client.views.chargen:update(secs)
	end}

Ui:add_widget{
	state = "chargen/body",
	widget = function() return Widgets.Uibutton("Back", function() Ui:pop_state() end) end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/face",
	init = function()
		local sliders = {
			{"Cheekbone size", true},
			{"Cheek size", true},
			{"Chin sharpness", false},
			{"Chin size", true},
			{"Eye inner", false},
			{"Eye distance", true},
			{"Eye outer", false},
			{"Eye size", true},
			{"Face roughness", false},
			{"Jaw pronounced", true},
			{"Jaw width", false},
			{"Lips protruded", false},
			{"Mouth width", false},
			{"Nose pointedness", true},
			{"Nose position", false}}
		-- Create the face shape sliders.
		local widgets = {}
		for k,v in ipairs(sliders) do
			local value = Client.views.chargen:get_face(k)
			if v[2] then value = 1 - value end
			local widget = Widgets.Uiscrollfloat(v[1], 0, 1, value, function(w)
				Client.views.chargen:set_face(k, w.inverse and 1 - w.value or w.value)
			end)
			widget.inverse = v[2]
			table.insert(widgets, widget)
		end
		return widgets
	end,
	update = function(secs)
		Client.views.chargen:update(secs)
	end}

Ui:add_widget{
	state = "chargen/face",
	widget = function() return Widgets.Uibutton("Back", function() Ui:pop_state() end) end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/misc",
	update = function(secs)
		Client.views.chargen:update(secs)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function() return Widgets.Uitransition("Hair style", "chargen/misc/hairstyle") end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.views.chargen:get_hair_color(1)
		return Widgets.Uiscrollfloat("Hair hue", 0, 1, value, function(w)
			Client.views.chargen:set_hair_color(1, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.views.chargen:get_hair_color(2)
		return Widgets.Uiscrollfloat("Hair saturation", 0, 1, value, function(w)
			Client.views.chargen:set_hair_color(2, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.views.chargen:get_hair_color(3)
		return Widgets.Uiscrollfloat("Hair lightness", 0, 1, value, function(w)
			Client.views.chargen:set_hair_color(3, w.value)
		end)
	end}

--[[Ui:add_widget{
	state = "chargen/misc",
	widget = function() return Widgets.Uitransition("Eye style", "chargen/misc/eyestyle") end}]]

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.views.chargen:get_eye_color(1)
		return Widgets.Uiscrollfloat("Eye hue", 0, 1, value, function(w)
			Client.views.chargen:set_eye_color(1, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.views.chargen:get_eye_color(2)
		return Widgets.Uiscrollfloat("Eye saturation", 0, 1, value, function(w)
			Client.views.chargen:set_eye_color(2, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.views.chargen:get_eye_color(3)
		return Widgets.Uiscrollfloat("Eye lightness", 0, 1, value, function(w)
			Client.views.chargen.set_eye_color(3, w.value)
		end)
	end}

--[[Ui:add_widget{
	state = "chargen/misc",
	widget = function() return Widgets.Uitransition("Skin style", "chargen/misc/skinstyle") end}]]

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.views.chargen:get_skin_color(1)
		return Widgets.Uiscrollfloat("Skin hue", 0, 1, value, function(w)
			Client.views.chargen:set_skin_color(1, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.views.chargen:get_skin_color(2)
		return Widgets.Uiscrollfloat("Skin saturation", 0, 1, value, function(w)
			Client.views.chargen:set_skin_color(2, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.views.chargen:get_skin_color(3)
		return Widgets.Uiscrollfloat("Skin lightness", 0, 1, value, function(w)
			Client.views.chargen:set_skin_color(3, w.value)
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/misc/hairstyle",
	init = function()
		local widgets = {}
		local spec = Client.views.chargen:get_race()
		for k,v in ipairs(spec.hair_styles) do
			local widget = Widgets.Uiradio(v[1], "hair", function(w)
				Client.views.chargen:set_hair_style(w.style)
			end)
			widget.style = v[2]
			if Client.views.chargen.hair_style == widget.style then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	update = function(secs)
		Client.views.chargen:update(secs)
	end}

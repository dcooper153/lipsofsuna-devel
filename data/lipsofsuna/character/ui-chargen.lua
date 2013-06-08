local Client = require("core/client/client")

local chargen_input = function(args)
	return Client.chargen:input(args)
end

local chargen_update = function(secs)
	Client.chargen:update(secs)
end

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen",
	root = "chargen",
	label = "Create character",
	init = function() Client.lighting:set_dungeon_mode(false) end,
	exit_root = function() Client.chargen:reset() end,
	input = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen",
	widget = function()
		local widget = Widgets.Uientry("Name", function(w)
			Client.chargen:set_name(w.value)
		end)
		widget.value = Client.chargen:get_name()
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
	widget = function() return Widgets.Uitransition("Start point", "chargen/spawnpoint") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uibutton("Create", function()
			Client.chargen:apply()
		end)
	end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Quit", "quit") end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/race",
	label = "Select race",
	init = function()
		-- Create the race radio buttons.
		local widgets = {}
		local races = Client.chargen:get_races()
		for k,v in ipairs(races) do
			local widget = Widgets.Uiradio(v[1], "race", function(w)
				Client.chargen:set_race(w.race)
			end)
			widget.race = v[2]
			if widget.race == Client.chargen:get_race() then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input = chargen_input,
	update = chargen_update}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/presets",
	label = "Select preset",
	init = function()
		-- Create the preset radio buttons.
		local widgets = {}
		local presets = Client.chargen:get_presets()
		for k,v in ipairs(presets) do
			local widget = Widgets.Uiradio(v.name, "preset", function(w)
				Client.chargen:set_preset(w.preset)
			end)
			widget.preset = v
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input = chargen_input,
	update = chargen_update}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/body",
	label = "Customize body",
	input = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen/body",
	widget = function() return Widgets.Uitransition("Skin style", "chargen/body/skinstyle") end}

Ui:add_widget{
	state = "chargen/body",
	widget = function()
		local sliders = {
			{"Arm muscularity", false},
			{"Body width", true},
			{"Bust size", false},
			{"Hips width", false},
			{"Leg muscularity", false},
			{"Torso width", false},
			{"Waist fatness", false},
			{"Waist width", false},
			{"Shoulder width", false}}
		-- Create the body proportion sliders.
		local widgets = {}
		for k,v in ipairs(sliders) do
			local value = Client.chargen:get_body(k)
			if v[2] then value = 1 - value end
			local widget = Widgets.Uiscrollfloat(v[1], 0, 1, value, function(w)
				Client.chargen:set_body(k, w.inverse and 1 - w.value or w.value)
			end)
			widget.inverse = v[2]
			table.insert(widgets, widget)
		end
		return widgets
	end}

Ui:add_widget{
	state = "chargen/body",
	widget = function()
		local value = Client.chargen:get_height()
		return Widgets.Uiscrollfloat("Height", 0, 1, value, function(w)
			Client.chargen:set_height(w.value)
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/body/skinstyle",
	label = "Select skin style",
	init = function()
		local widgets = {}
		local race = Client.chargen:get_race()
		local specs = Actorskinspec:find_by_actor(race)
		for k,v in ipairs(specs) do
			local widget = Widgets.Uiradio(v.name, "skin", function(w)
				Client.chargen:set_skin_style(w.style)
			end)
			widget.style = v.name
			if Client.chargen:get_skin_style() == widget.style then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input = chargen_input,
	update = chargen_update}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/face",
	label = "Customize face",
	input = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen/face",
	widget = function() return Widgets.Uitransition("Head style", "chargen/face/headstyle") end}

Ui:add_widget{
	state = "chargen/face",
	widget = function()
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
			local value = Client.chargen:get_face(k)
			if v[2] then value = 1 - value end
			local widget = Widgets.Uiscrollfloat(v[1], 0, 1, value, function(w)
				Client.chargen:set_face(k, w.inverse and 1 - w.value or w.value)
			end)
			widget.inverse = v[2]
			table.insert(widgets, widget)
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/face/headstyle",
	label = "Select head style",
	init = function()
		local widgets = {}
		local race = Client.chargen:get_race()
		local spec = Actorspec:find{name = race}
		if not spec.head_styles then return end
		local lst = {}
		for k,v in pairs(spec.head_styles) do
			table.insert(lst, {k, v})
		end
		table.sort(lst, function(a,b) return a[1] < b[1] end)
		for k,v in ipairs(lst) do
			local widget = Widgets.Uiradio(v[1], "head", function(w)
				Client.chargen:set_head_style(w.style)
			end)
			widget.style = v[2]
			if Client.chargen:get_head_style() == widget.style then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input = chargen_input,
	update = chargen_update}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/misc",
	label = "Customize other",
	input = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen/misc",
	widget = function() return Widgets.Uitransition("Animation profile", "chargen/misc/animation") end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function() return Widgets.Uitransition("Hair style", "chargen/misc/hairstyle") end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_hair_color(1)
		return Widgets.Uiscrollfloat("Hair hue", 0, 1, value, function(w)
			Client.chargen:set_hair_color(1, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_hair_color(2)
		return Widgets.Uiscrollfloat("Hair saturation", 0, 1, value, function(w)
			Client.chargen:set_hair_color(2, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_hair_color(3)
		return Widgets.Uiscrollfloat("Hair lightness", 0, 1, value, function(w)
			Client.chargen:set_hair_color(3, w.value)
		end)
	end}

--[[Ui:add_widget{
	state = "chargen/misc",
	widget = function() return Widgets.Uitransition("Eye style", "chargen/misc/eyestyle") end}]]

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_eye_color(1)
		return Widgets.Uiscrollfloat("Eye hue", 0, 1, value, function(w)
			Client.chargen:set_eye_color(1, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_eye_color(2)
		return Widgets.Uiscrollfloat("Eye saturation", 0, 1, value, function(w)
			Client.chargen:set_eye_color(2, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_eye_color(3)
		return Widgets.Uiscrollfloat("Eye lightness", 0, 1, value, function(w)
			Client.chargen:set_eye_color(3, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_skin_color(1)
		return Widgets.Uiscrollfloat("Skin hue", 0, 1, value, function(w)
			Client.chargen:set_skin_color(1, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_skin_color(2)
		return Widgets.Uiscrollfloat("Skin saturation", 0, 1, value, function(w)
			Client.chargen:set_skin_color(2, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_skin_color(3)
		return Widgets.Uiscrollfloat("Skin lightness", 0, 1, value, function(w)
			Client.chargen:set_skin_color(3, w.value)
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/misc/animation",
	label = "Select animation",
	init = function()
		local widgets = {}
		local race = Client.chargen:get_race()
		local spec = Actorspec:find{name = race}
		if not spec.animations then return end
		local lst = {}
		for k,v in pairs(spec.animations) do
			table.insert(lst, {k, v})
		end
		table.sort(lst, function(a,b) return a[1] < b[1] end)
		for k,v in ipairs(lst) do
			local widget = Widgets.Uiradio(v[1], "animations", function(w)
				Client.chargen:set_animation_profile(w.profile)
			end)
			widget.profile = v[2]
			if Client.chargen:get_animation_profile() == widget.style then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input = chargen_input,
	update = chargen_update}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/misc/hairstyle",
	label = "Select hair style",
	init = function()
		local widgets = {}
		local race = Client.chargen:get_race()
		local spec = Actorspec:find{name = race}
		if not spec.hair_styles then return end
		local lst = {}
		for k,v in pairs(spec.hair_styles) do
			table.insert(lst, {k, v})
		end
		table.sort(lst, function(a,b) return a[1] < b[1] end)
		for k,v in ipairs(lst) do
			local widget = Widgets.Uiradio(v[1], "hair", function(w)
				Client.chargen:set_hair_style(w.style)
			end)
			widget.style = v[2]
			if Client.chargen:get_hair_style() == widget.style then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input = chargen_input,
	update = chargen_update}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/spawnpoint",
	label = "Select start point",
	init = function()
		-- Get the spawn point list.
		local spawnpoints = {}
		for k,v in ipairs(Patternspec:find_spawn_points()) do
			table.insert(spawnpoints, v.name)
		end
		table.sort(spawnpoints, function(a,b) return a < b end)
		table.insert(spawnpoints, 1, "Home")
		-- Create the widgets.
		local current = Client.chargen:get_spawn_point()
		local widgets = {}
		for k,v in ipairs(spawnpoints) do
			local widget = Widgets.Uiradio(v, "spawn", function(w)
				Client.chargen:set_spawn_point(w.spawnpoint)
			end)
			widget.spawnpoint = v
			if widget.spawnpoint == current then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input = chargen_input,
	update = chargen_update}

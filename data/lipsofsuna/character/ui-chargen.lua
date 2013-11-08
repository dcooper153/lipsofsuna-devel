local ActorTextureSpec = require("core/specs/actor-texture")
local ChargenSliderSpec = require("core/specs/chargen-slider")
local Client = require("core/client/client")
local HairStyleSpec = require("core/specs/hair-style")
local UiRadioMenu = require("ui/widgets/radio-menu")
local UiScrollFloat = require("ui/widgets/scrollfloat")

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
	input_post = chargen_input,
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
	widget = function()
		return UiRadioMenu("Preset", function(self)
			-- Find and sort the presets.
			local lst = Client.chargen:get_presets()
			-- Create the popup widgets.
			self:clear()
			for k,v in ipairs(lst) do
				self:add_item(v.name, false, function(w)
					Client.chargen:set_preset(v)
				end)
			end
			self:add_widget(Widgets.Uibutton("Save the current character", function()
				Client.chargen:save()
			end))
		end)
	end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Body", "chargen/body") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Head", "chargen/head") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Face", "chargen/face") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Misc", "chargen/misc") end}

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
	state = "chargen/body",
	label = "Customize body",
	input_post = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen/body",
	widget = function()
		return UiRadioMenu("Skin style", function(self)
			-- Create the popup widgets.
			local widgets = {}
			local race = Client.chargen:get_race()
			local specs = Actorskinspec:find_by_actor(race)
			for k,v in ipairs(specs) do
				local active = Client.chargen:get_skin_style() == v.name
				self:add_item(v.name, active, function(w)
					Client.chargen:set_skin_style(v.name)
				end)
			end
		end)
	end}

Ui:add_widget{
	state = "chargen/body",
	widget = function()
		-- Create the body proportion sliders.
		local widgets = {}
		for k,v in ipairs(ChargenSliderSpec:find_by_category("body")) do
			local value = Client.chargen:get_body(v.field_index) or 0
			local widget = UiScrollFloat(v.name, 0, 1, value, function(w)
				Client.chargen:set_body(k, w.value)
			end)
			table.insert(widgets, widget)
		end
		return widgets
	end}

Ui:add_widget{
	state = "chargen/body",
	widget = function()
		local value = Client.chargen:get_height()
		return UiScrollFloat("Height", 0, 1, value, function(w)
			Client.chargen:set_height(w.value)
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/head",
	label = "Customize head",
	input_post = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen/head",
	widget = function()
		return UiRadioMenu("Head style", function(self)
			-- Find and sort the head styles.
			local race = Client.chargen:get_race()
			local spec = Actorspec:find_by_name(race)
			if not spec.head_styles then return end
			local lst = {}
			for k,v in pairs(spec.head_styles) do
				table.insert(lst, {k, v})
			end
			table.sort(lst, function(a,b) return a[1] < b[1] end)
			-- Create the popup widgets.
			self:clear()
			for k,v in ipairs(lst) do
				local active = Client.chargen:get_head_style() == v[2]
				self:add_item(v[1], active, function(w)
					Client.chargen:set_head_style(v[2])
					self:set_menu_opened(false)
				end)
			end
		end)
	end}

Ui:add_widget{
	state = "chargen/head",
	widget = function()
		return UiRadioMenu("Hair style", function(self)
			-- Find and sort the hair styles.
			local lst = {}
			for k,v in pairs(HairStyleSpec.dict_name) do
				table.insert(lst, {k, k})
			end
			table.sort(lst, function(a,b) return a[1] < b[1] end)
			-- Create the popup widgets.
			self:clear()
			for k,v in ipairs(lst) do
				local active = Client.chargen:get_head_style() == v[2]
				self:add_item(v[1], active, function(w)
					Client.chargen:set_hair_style(v[2])
				end)
			end
		end)
	end}

Ui:add_widget{
	state = "chargen/head",
	widget = function()
		local value = Client.chargen:get_hair_color(1)
		return UiScrollFloat("Hair hue", 0, 1, value, function(w)
			Client.chargen:set_hair_color(1, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/head",
	widget = function()
		local value = Client.chargen:get_hair_color(2)
		return UiScrollFloat("Hair saturation", 0, 1, value, function(w)
			Client.chargen:set_hair_color(2, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/head",
	widget = function()
		local value = Client.chargen:get_hair_color(3)
		return UiScrollFloat("Hair lightness", 0, 1, value, function(w)
			Client.chargen:set_hair_color(3, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/head",
	widget = function()
		local value = Client.chargen:get_head_scale()
		return UiScrollFloat("Head size", 0, 1, value, function(w)
			Client.chargen:set_head_scale(w.value)
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/face",
	label = "Customize face",
	input_post = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen/face",
	widget = function()
		return UiRadioMenu("Eye style", function(self)
			-- Find and sort the eye styles.
			local race = Client.chargen:get_race()
			local lst = ActorTextureSpec:find_by_actor_and_usage(race, "eyes")
			-- Create the popup widgets.
			self:clear()
			for k,v in ipairs(lst) do
				local active = Client.chargen:get_eye_style() == v.name
				self:add_item(v.label, active, function(w)
					Client.chargen:set_eye_style(v.name)
				end)
			end
		end)
	end}

Ui:add_widget{
	state = "chargen/face",
	widget = function()
		local value = Client.chargen:get_eye_color(1)
		return UiScrollFloat("Eye hue", 0, 1, value, function(w)
			Client.chargen:set_eye_color(1, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/face",
	widget = function()
		local value = Client.chargen:get_eye_color(2)
		return UiScrollFloat("Eye saturation", 0, 1, value, function(w)
			Client.chargen:set_eye_color(2, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/face",
	widget = function()
		local value = Client.chargen:get_eye_color(3)
		return UiScrollFloat("Eye lightness", 0, 1, value, function(w)
			Client.chargen:set_eye_color(3, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/face",
	widget = function()
		-- Create the face shape sliders.
		local widgets = {}
		for k,v in ipairs(ChargenSliderSpec:find_by_category("face")) do
			local value = Client.chargen:get_face(v.field_index) or 0
			local widget = UiScrollFloat(v.name, 0, 1, value, function(w)
				Client.chargen:set_face(k, w.value)
			end)
			table.insert(widgets, widget)
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/misc",
	label = "Customize other",
	input_post = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		return UiRadioMenu("Animation profile", function(self)
			-- Find and sort the profiles.
			local race = Client.chargen:get_race()
			local spec = Actorspec:find{name = race}
			if not spec.animations then return end
			local lst = {}
			for k,v in pairs(spec.animations) do
				table.insert(lst, {k, v})
			end
			table.sort(lst, function(a,b) return a[1] < b[1] end)
			-- Create the popup widgets.
			self:clear()
			for k,v in ipairs(lst) do
				local active = Client.chargen:get_animation_profile() == v[2]
				self:add_item(v[1], active, function(w)
					Client.chargen:set_animation_profile(v[2])
				end)
			end
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_skin_color(1)
		return UiScrollFloat("Skin hue", 0, 1, value, function(w)
			Client.chargen:set_skin_color(1, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_skin_color(2)
		return UiScrollFloat("Skin saturation", 0, 1, value, function(w)
			Client.chargen:set_skin_color(2, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_skin_color(3)
		return UiScrollFloat("Skin lightness", 0, 1, value, function(w)
			Client.chargen:set_skin_color(3, w.value)
		end)
	end}

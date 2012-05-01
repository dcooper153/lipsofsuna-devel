Ui:add_state{
	state = "introspect",
	label = "Introspect",
	init = function()
		Operators.introspect:init()
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Actor specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Actorspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Actor preset specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Actorpresetspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Animation specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Animationspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Constraint specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Constraintspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Dialog specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Dialogspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Effect specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Effectspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Faction specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Factionspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Help specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Helpspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Icon specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Iconspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Item specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Itemspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Obstacle specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Obstaclespec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Pattern specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Patternspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Personality specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Personalityspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Quest specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Questspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Spell specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Spellspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Skill specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Skillspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Static specs", "introspect/specs",
		function() Operators.introspect:set_spec_type("Staticspec") end)
	end}

Ui:add_widget{
	state = "introspect",
	widget = function() return Widgets.Uitransition("Quit", "quit") end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "introspect/specs",
	label = "Specs"}

Ui:add_state{
	state = "introspect/specs",
	init = function()
		local specs = Operators.introspect:get_specs()
		local widgets = {}
		for k,v in ipairs(specs) do
			widgets[k] = Widgets.Uitransition(v.name, "introspect/spec", function()
				Operators.introspect:set_spec_name(v.name)
			end)
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "introspect/spec",
	label = "Spec"}

Ui:add_state{
	state = "introspect/spec",
	init = function()
		local spec = Operators.introspect:get_spec()
		local fields = Operators.introspect:get_fields()
		local widgets = {}
		for k,v in ipairs(fields) do
			if v.type ~= "ignore" then
				table.insert(widgets, Widgets.Uiintrospectfield(spec, v))
			end
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "introspect/field",
	label = "Field"}

Ui:add_state{
	state = "introspect/field",
	init = function()
		local spec = Operators.introspect:get_spec()
		local field = Operators.introspect:get_field()
		local value = spec[field.name]
		local widgets = {}
		table.insert(widgets, Widgets.Uilabel("Name: " .. field.name))
		table.insert(widgets, Widgets.Uilabel("Type: " .. field.type))
		table.insert(widgets, Widgets.Uilabel("Value: " .. tostring(value)))
		table.insert(widgets, Widgets.Uilabel("Default: " .. tostring(field.default)))
		return widgets
	end}

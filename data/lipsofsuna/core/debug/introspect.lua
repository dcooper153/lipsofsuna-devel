local ChatCommand = require("core/chat/chat-command")
local Introspect = require("core/introspect/introspect")
local IntrospectOperator = require("core/debug/introspect-operator")
local Spec = require("core/specs/spec")

ChatCommand{
	name = "introspect",
	description = "Browse specs.",
	pattern = "^/introspect$",
	permission = "player",
	handler = "client",
	func = function(player, matches)
		Ui:set_state("introspect")
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "introspect",
	label = "Introspect",
	init = function()
		IntrospectOperator:init()
	end}

Ui:add_widget{
	state = "introspect",
	widget = function()
		local list = {}
		for k in pairs(Spec.dict_spec) do
			table.insert(list, k)
		end
		table.sort(list)
		local widgets = {}
		for k,v in ipairs(list) do
			table.insert(widgets, Widgets.Uitransition(v, "introspect/specs",
				function() IntrospectOperator:set_spec_type(v) end))
		end
		return widgets
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
		local specs = IntrospectOperator:get_specs()
		local widgets = {}
		for k,v in ipairs(specs) do
			widgets[k] = Widgets.Uitransition(v.name, "introspect/spec", function()
				IntrospectOperator:set_spec_name(v.name)
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
		local spec = IntrospectOperator:get_spec()
		local fields = IntrospectOperator:get_fields()
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
		local spec = IntrospectOperator:get_spec()
		local field = IntrospectOperator:get_field()
		local value = spec[field.name]
		local widgets = {}
		table.insert(widgets, Widgets.Uilabel("Name: " .. field.name))
		table.insert(widgets, Widgets.Uilabel("Type: " .. field.type))
		table.insert(widgets, Widgets.Uilabel("Value: " .. tostring(value)))
		table.insert(widgets, Widgets.Uilabel("Default: " .. tostring(field.default)))
		return widgets
	end}
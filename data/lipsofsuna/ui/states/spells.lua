local ModifierSpec = require("core/specs/modifier")

Ui:add_state{
	state = "spells",
	label = "Spells",
	init = function()
		local spells = Operators.spells:get_spells()
		local current = Operators.spells:get_spell_index()
		local widgets = {}
		for k,v in ipairs(spells) do
			widgets[k] = Widgets.Uispellpreview(k, v)
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "spells/spell",
	label = "Edit spell"}

Ui:add_widget{
	state = "spells/spell",
	widget = function()
		local spell = Operators.spells:get_spell()
		return Widgets.Uispellslot("type", spell and spell.animation)
	end}

Ui:add_widget{
	state = "spells/spell",
	widget = function()
		local widgets = {}
		for i = 1,3 do
			local spell = Operators.spells:get_spell()
			local effect = spell.effects[i]
			table.insert(widgets, Widgets.Uispellslot("effect", effect and effect[1], i))
		end
		return widgets
	end}

Ui:add_widget{
	state = "spells/spell",
	widget = function()
		-- Calculate reagent requirements.
		local spell = Operators.spells:get_spell()
		local info = spell:get_info()
		local text = ""
		if info then
			local list = {}
			for k,v in pairs(info.required_reagents) do
				table.insert(list, k .. ": " .. v)
			end
			table.sort(list)
			for k,v in ipairs(list) do
				text = text .. (text ~= "" and "\n" or "") .. v
			end
		end
		-- Create the text widget.
		text = "Required reagents:\n" .. text
		return Widgets.Uilabel(text) end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "spells/types",
	label = "Select type",
	init = function()
		-- Create the list of types.
		local types = {}
		for name,spec in pairs(Feattypespec.dict_name) do
			if Client.data.unlocks:get("spell type", name) then
				table.insert(types, name)
			end
		end
		table.sort(types)
		-- Create the widgets.
		local widgets = {}
		for k,v in ipairs(types) do
			local widget = Widgets.Uispell("type", v, true)
			table.insert(widgets, widget)
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "spells/effects",
	label = "Select effect",
	init = function()
		-- Get the spell type spec.
		local spell = Operators.spells:get_spell()
		local anim = Feattypespec:find{name = spell.animation}
		if not anim then return end
		-- Create the list of effects.
		local effects = {}
		for name in pairs(ModifierSpec.dict_name) do
			if Client.data.unlocks:get("spell effect", name) then
				table.insert(effects, name)
			end
		end
		table.sort(effects)
		-- Create the widgets.
		local widgets = {}
		for k,v in ipairs(effects) do
			local widget = Widgets.Uispell("effect", v, anim and anim.effects[v])
			table.insert(widgets, widget)
		end
		return widgets
	end}


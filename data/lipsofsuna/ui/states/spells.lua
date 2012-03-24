Ui:add_state{
	state = "spells",
	label = "Spells"}

Ui:add_widget{
	state = "spells",
	widget = function()
		local slot = Client.data.spells.slot
		return Widgets.Uiscrollinteger("Slot", 1, 10, slot, function(w)
			Client.data.spells.slot = w.value
			Ui:restart_state()
		end)
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		local w = Widgets.Uiradio("Self", "type", function()
			Client:set_spell(Client.data.spells.slot, "spell on self")
			Ui:restart_state()
		end)
		local spell = Client:get_spell(Client.data.spells.slot)
		if spell.animation == "spell on self" then
			w.value = true
		end
		return w
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		local w = Widgets.Uiradio("Ranged", "type", function()
			Client:set_spell(Client.data.spells.slot, "ranged spell")
			Ui:restart_state()
		end)
		local spell = Client:get_spell(Client.data.spells.slot)
		if spell.animation == "ranged spell" then
			w.value = true
		end
		return w
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		local w = Widgets.Uiradio("Touch", "type", function()
			Client:set_spell(Client.data.spells.slot, "spell on touch")
			Ui:restart_state()
		end)
		local spell = Client:get_spell(Client.data.spells.slot)
		if spell.animation == "spell on touch" then
			w.value = true
		end
		return w
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		local spell = Client:get_spell(Client.data.spells.slot)
		local effect = spell.effects[1]
		return Widgets.Uispellslot(effect and effect[1], 1)
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		local spell = Client:get_spell(Client.data.spells.slot)
		local effect = spell.effects[2]
		return Widgets.Uispellslot(effect and effect[1], 2)
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		local spell = Client:get_spell(Client.data.spells.slot)
		local effect = spell.effects[3]
		return Widgets.Uispellslot(effect and effect[1], 3)
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		-- Calculate reagent requirements.
		local spell = Client:get_spell(Client.data.spells.slot)
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
	state = "spells/effects",
	label = "Select effect",
	init = function()
		-- Get the player actor spec.
		local spec = Client.player_object.spec
		if not spec then return end
		-- Get the spell type spec.
		local spell = Client:get_spell(Client.data.spells.slot)
		local anim = Featanimspec:find{name = spell.animation}
		if not anim then return end
		-- Create the list of effects.
		local effects = {}
		for k in pairs(spec.feat_effects) do
			local e = Feateffectspec:find{name = k}
			if e and e.description then
				table.insert(effects, k)
			end
		end
		table.sort(effects)
		-- Create the widgets.
		local widgets = {}
		for k,v in ipairs(effects) do
			local widget = Widgets.Uispell(v, anim and anim.effects[v])
			table.insert(widgets, widget)
		end
		return widgets
	end}

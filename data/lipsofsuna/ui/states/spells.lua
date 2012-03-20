Ui:add_state{
	state = "spells",
	label = "Spells"}

Ui:add_widget{
	state = "spells",
	widget = function()
		local slot = Client.views.feats.slot
		return Widgets.Uiscrollinteger("Slot", 1, 10, slot, function(w)
			Client.views.feats:show(w.value)
			Ui:restart_state()
		end)
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		local w = Widgets.Uiradio("Self", "type", function()
			Client.views.feats:set_anim("spell on self")
			Ui:restart_state()
		end)
		if Client.views.feats:get_anim() == "spell on self" then
			w.value = true
		end
		return w
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		local w = Widgets.Uiradio("Ranged", "type", function()
			Client.views.feats:set_anim("ranged spell")
			Ui:restart_state()
		end)
		if Client.views.feats:get_anim() == "ranged spell" then
			w.value = true
		end
		return w
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		local w = Widgets.Uiradio("Touch", "type", function()
			Client.views.feats:set_anim("spell on touch")
			Ui:restart_state()
		end)
		if Client.views.feats:get_anim() == "spell on touch" then
			w.value = true
		end
		return w
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		local effect = Client.views.feats:get_effect(1)
		return Widgets.Uispellslot(effect, 1)
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		local effect = Client.views.feats:get_effect(2)
		return Widgets.Uispellslot(effect, 2)
	end}

Ui:add_widget{
	state = "spells",
	widget = function()
		local effect = Client.views.feats:get_effect(3)
		return Widgets.Uispellslot(effect, 3)
	end}

Ui:add_widget{
	state = "spells",
	widget = function() return Widgets.Uibutton("Apply", function()
			Client.views.feats:assign()
		end)
	end}

Ui:add_widget{
	state = "spells",
	widget = function() return Widgets.Uilabel(Client.views.feats.skills_text) end}

Ui:add_widget{
	state = "spells",
	widget = function() return Widgets.Uilabel(Client.views.feats.reagents_text) end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "spells/effects",
	label = "Select effect",
	init = function()
		local spec = Client.player_object.spec
		if not spec then return end
		local anim = Featanimspec:find{name = Client.views.feats:get_anim()}
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

require "client/quickslots"

Views.Feats = Class(Widget)
Views.Feats.mode = "chargen"

--- Creates a new feat editor.
-- @param clss Feats class.
-- @return Feats.
Views.Feats.new = function(clss)
	local self = Widget.new(clss, {cols = 1, spacings = {0,0,0,0}})
	-- Title.
	self.title = Widgets.Frame{style = "title", text = "Feats"}
	-- Animation selector.
	local label = Widgets.Label{text = "Type"}
	label:set_request{width = 46}
	self.combo_anim = Widgets.ComboBox()
	self.group_anim = Widget{cols = 2, rows = 1}
	self.group_anim:set_expand{col = 2}
	self.group_anim:set_child{col = 1, row = 1, widget = label}
	self.group_anim:set_child{col = 2, row = 1, widget = self.combo_anim}
	-- Effect selectors.
	self.combo_effect = {}
	self.scroll_effect = {}
	for i = 1,3 do
		self.combo_effect[i] = Widgets.ComboBox()
		self.combo_effect[i]:set_request{width = 150}
		self.scroll_effect[i] = Widgets.Progress{min = 0, max = 100, value = 0, pressed = function(w)
			w.value = w:get_value_at(Client.cursor_pos)
			self:changed()
		end}
		self.scroll_effect[i]:set_request{width = 100}
	end
	self.group_effect = Widget{cols = 3, rows = 3}
	self.group_effect:set_expand{col = 3}
	self.group_effect:set_child{col = 1, row = 1, widget = Widgets.Label{text = "Effect 1"}}
	self.group_effect:set_child{col = 1, row = 2, widget = Widgets.Label{text = "Effect 2"}}
	self.group_effect:set_child{col = 1, row = 3, widget = Widgets.Label{text = "Effect 3"}}
	self.group_effect:set_child{col = 2, row = 1, widget = self.combo_effect[1]}
	self.group_effect:set_child{col = 2, row = 2, widget = self.combo_effect[2]}
	self.group_effect:set_child{col = 2, row = 3, widget = self.combo_effect[3]}
	self.group_effect:set_child{col = 3, row = 1, widget = self.scroll_effect[1]}
	self.group_effect:set_child{col = 3, row = 2, widget = self.scroll_effect[2]}
	self.group_effect:set_child{col = 3, row = 3, widget = self.scroll_effect[3]}
	-- Information display.
	self.label_skills = Widgets.Label{valign = 0, color = {0,0,0,1}}
	self.label_reagents = Widgets.Label{valign = 0, color = {0,0,0,1}}
	self.label_description = Widgets.Label{valign = 0, color = {0,0,0,1}}
	self.label_description:set_request{height = 80, width = 270}
	self.group_req = Widget{rows = 2, cols = 2}
	self.group_req:set_expand{col = 1, row = 2}
	self.group_req:set_expand{col = 2}
	self.group_req:set_child{row = 1, col = 1, widget = Widgets.Label{font = "medium", text = "Skills", color = {0,0,0,1}}}
	self.group_req:set_child{row = 1, col = 2, widget = Widgets.Label{font = "medium", text = "Reagents", color = {0,0,0,1}}}
	self.group_req:set_child{row = 2, col = 1, widget = self.label_skills}
	self.group_req:set_child{row = 2, col = 2, widget = self.label_reagents}
	self.group_info = Widgets.Frame{rows = 3, cols = 1, style = "paper"}
	self.group_info:set_request{height = 310}
	self.group_info:set_expand{col = 1, row = 3}
	self.group_info:set_child{row = 1, col = 1, widget = Widgets.Label{font = "medium", text = "Description", color = {0,0,0,1}}}
	self.group_info:set_child{row = 2, col = 1, widget = self.label_description}
	self.group_info:set_child{row = 3, col = 1, widget = self.group_req}
	-- Quickslot selector.
	self.combo_slot = Widgets.ComboBox()
	for i = 1,12 do
		self.combo_slot:append{text = "Quickslot " .. i, pressed = function() self:show(i) end}
	end
	local label1 = Widgets.Label{text = "Slot"}
	label1:set_request{width = 46}
	self.group_slot = Widget{cols = 2, rows = 1}
	self.group_slot:set_expand{col = 2}
	self.group_slot:set_child{col = 1, row = 1, widget = label1}
	self.group_slot:set_child{col = 2, row = 1, widget = self.combo_slot}
	-- Assign button.
	self.button_assign = Widgets.Button{text = "Assign to the quickslot"}
	self.button_assign.pressed = function() self:assign() end
	-- Packing for the dialog.
	self.group = Widgets.Frame{cols = 1}
	self.group:append_row(self.group_slot)
	self.group:append_row(self.group_anim)
	self.group:append_row(self.group_effect)
	self.group:append_row(self.button_assign)
	self:append_row(self.title)
	self:append_row(self.group)
	self:append_row(self.group_info)
	-- Show the first feat.
	self:show(1)
	return self
end

Views.Feats.assign = function(self)
	-- Get effects and their magnitudes.
	local effects = {}
	local values = {}
	for i = 1,3 do
		table.insert(effects, self.combo_effect[i].text)
		table.insert(values, self.scroll_effect[i].value)
	end
	-- Create a feat from the animation and the effects.
	local feat = Feat{animation = self.combo_anim.text ~= "" and self.combo_anim.text, effects = {}}
	for i = 1,3 do
		feat.effects[i] = {effects[i], values[i]}
	end
	Quickslots:assign_feat(self.active_slot, feat)
end

Views.Feats.back = function(self)
	Gui:set_mode("menu")
end

--- Recalculates the skill and reagent requirements of the currently shown feat.
-- @param self Feats.
Views.Feats.changed = function(self)
	if self.protect then return end
	-- Get effects and their magnitudes.
	local effects = {}
	local values = {}
	for i = 1,3 do
		table.insert(effects, self.combo_effect[i].text)
		table.insert(values, self.scroll_effect[i].value)
	end
	-- Calculate skill and reagent requirements.
	local reagents = {}
	local skills = {}
	for index,name in pairs(effects) do
		local effect = Feateffectspec:find{name = name}
		if effect then
			-- Base skill requirements.
			for skill,value in pairs(effect.skill_base) do
				local val = skills[skill] or 0
				skills[skill] = val + value
			end
			-- Magnitude based skill requirements.
			for skill,mult in pairs(effect.skill_mult) do
				local val = skills[skill] or 0
				skills[skill] = val + mult * values[index]
			end
			-- Base reagent requirements.
			for reagent,value in pairs(effect.reagent_base) do
				local val = reagents[reagent] or 0
				reagents[reagent] = val + value
			end
			-- Magnitude based reagent requirements.
			for reagent,mult in pairs(effect.reagent_mult) do
				local val = reagents[reagent] or 0
				reagents[reagent] = val + mult * values[index]
			end
		end
	end
	for k,v in pairs(skills) do
		skills[k] = math.max(1, math.ceil(v))
	end
	for k,v in pairs(reagents) do
		reagents[k] = math.max(1, math.ceil(v))
	end
	-- Display skill requirements.
	local skill_list = {}
	for k,v in pairs(skills) do
		table.insert(skill_list, k .. ": " .. v)
	end
	table.sort(skill_list)
	local skill_str = ""
	for k,v in ipairs(skill_list) do
		skill_str = skill_str .. (skill_str ~= "" and "\n" or "") .. v
	end
	self.label_skills.text = skill_str
	-- Display reagent requirements.
	local reagent_list = {}
	for k,v in pairs(reagents) do
		table.insert(reagent_list, k .. ": " .. v)
	end
	table.sort(reagent_list)
	local reagent_str = ""
	for k,v in ipairs(reagent_list) do
		reagent_str = reagent_str .. (reagent_str ~= "" and "\n" or "") .. v
	end
	self.label_reagents.text = reagent_str
	-- Display Description.
	local desc = ""
	local anim = Featanimspec:find{name = self.combo_anim.text}
	if anim then
		desc = " - " .. anim.description
		for i = 1,3 do
			local name = self.combo_effect[i].text or ""
			local effect = Feateffectspec:find{name = name}
			if effect and effect.description then
				desc = desc .. "\n - " .. effect.description
			end
		end
	end
	self.label_description.text = desc
end

--- Sets the race of the character using the feat editor.
-- @param self Feats.
-- @param name Race name.
Views.Feats.set_race = function(self, name)
	local spec = Species:find{name = name}
	if not spec then return end
	-- Rebuild the feat animation list.
	self.dict_anims_id = {{"", nil}}
	for k in pairs(spec.feat_anims) do
		local anim = Featanimspec:find{name = k}
		if anim and anim.description then
			table.insert(self.dict_anims_id, {k, function() self:changed() end})
		end
	end
	table.sort(self.dict_anims_id, function(a, b) return a[1]<b[1] end)
	self.combo_anim:clear()
	for k,v in ipairs(self.dict_anims_id) do
		self.combo_anim:append{text = v[1], pressed = v[2]}
	end
	-- Rebuild the feat effect list.
	self.dict_effects_id = {{"", nil}}
	for k in pairs(spec.feat_effects) do
		table.insert(self.dict_effects_id, {k, function() self:changed() end})
	end
	table.sort(self.dict_effects_id, function(a, b) return a[1]<b[1] end)
	for i = 1,3 do
		self.combo_effect[i]:clear()
		for k,v in ipairs(self.dict_effects_id) do
			self.combo_effect[i]:append{text = v[1], pressed = v[2]}
		end
	end
end

--- Shows the feat for the given quickslot.
-- @param self Feats.
-- @param index Quickslot index.
Views.Feats.show = function(self, index)
	local feat = Quickslots.buttons[index].feat or Feat()
	self.protect = true
	self.combo_slot:activate{index = index, press = false}
	self.combo_anim:activate{text = feat.animation or ""}
	for j = 1,3 do
		local e = feat.effects[j]
		self.combo_effect[j]:activate{text = e and e[1] or ""}
		self.scroll_effect[j].value = e and e[2] or 0
	end
	self.protect = nil
	self.active_slot = index
	self:changed()
end

Views.Feats.inst = Views.Feats()

require(Mod.path .. "widget")

Widgets.Uispellpreview = Class(Widgets.Uiwidget)
Widgets.Uispellpreview.class_name = "Widgets.Uispellpreview"

Widgets.Uispellpreview.new = function(clss, index, spell)
	local self = Widgets.Uiwidget.new(clss)
	self.index = index
	self.spell = spell
	self.hint = "$A: Edit\n$$B\n$$U\n$$D" or "$$B\n$$U\n$$D"
	return self
end

Widgets.Uispellpreview.apply = function(self)
	Operators.spells:set_spell_index(self.index)
	Ui:push_state("spells/spell")
	Client.effects:play_global("uitransition1")
end

Widgets.Uispellpreview.rebuild_size = function(self)
	local size = Vector()
	size.x = Theme.text_height_1 * 1.6 + Theme.width_icon_1 * 5.2 + 10
	size.y = Theme.width_icon_1 + 10
	return size
end

Widgets.Uispellpreview.rebuild_canvas = function(self)
	local w = self.size.x
	local h = self.size.y
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the index label.
	self:canvas_text{
		dest_position = {5,5},
		dest_size = {w,h},
		text = string.format("%d.", self.index),
		text_alignment = {0,0.5},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_2}
	-- Add the icons.
	if self.spell then
		local x1 = 5 + Theme.text_height_1 * 1.6
		local x2 = x1 + Theme.width_icon_1 * 2
		local x3 = x2 + Theme.width_icon_1 * 1.1
		local x4 = x3 + Theme.width_icon_1 * 1.1
		local e1 = self.spell.effects[1]
		local e2 = self.spell.effects[2]
		local e3 = self.spell.effects[3]
		local spec1 = Feattypespec:find{name = self.spell.animation}
		local spec2 = e1 and Feateffectspec:find{name = e1[1]}
		local spec3 = e2 and Feateffectspec:find{name = e2[1]}
		local spec4 = e3 and Feateffectspec:find{name = e3[1]}
		if spec1 then
			Theme:draw_icon_scaled(self, spec1.icon,
				x1, 5, Theme.width_icon_1, Theme.width_icon_1)
		end
		if spec2 then
			Theme:draw_icon_scaled(self, spec2.icon,
				x2, 5, Theme.width_icon_1, Theme.width_icon_1)
		end
		if spec3 then
			Theme:draw_icon_scaled(self, spec3.icon,
				x3, 5, Theme.width_icon_1, Theme.width_icon_1)
		end
		if spec4 then
			Theme:draw_icon_scaled(self, spec4.icon,
				x4, 5, Theme.width_icon_1, Theme.width_icon_1)
		end
	end
end

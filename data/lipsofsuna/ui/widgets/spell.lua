local Class = require("system/class")
local ModifierSpec = require("core/specs/modifier")
require(Mod.path .. "widget")

Widgets.Uispell = Class("Uispell", Widgets.Uiwidget)

Widgets.Uispell.new = function(clss, mode, name, active)
	local self = Widgets.Uiwidget.new(clss)
	if mode == "type" then
		self.spec = Feattypespec:find{name = name}
	else
		self.spec = ModifierSpec:find{name = name}
	end
	self.mode = mode
	self.active = active
	self.hint = active and "$A: Select\n$$B\n$$U\n$$D" or "$$B\n$$U\n$$D"
	self.icon = self.spec and Iconspec:find{name = self.spec.icon}
	return self
end

Widgets.Uispell.apply = function(self)
	if not self.active then return end
	if self.mode == "type" then
		Operators.spells:set_spell(self.spec.name)
	else
		local spell = Operators.spells:get_spell()
		Operators.spells:set_effect{self.spec.name, 1}
	end
	Ui:pop_state()
	Client.effects:play_global("uitransition1")
end

Widgets.Uispell.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the description.
	if self.spec then
		local w1,h1 = Program:measure_text(Theme.text_font_2, self:get_pretty_name(), size.x-5-Theme.width_icon_1)
		local w2,h2 = Program:measure_text(Theme.text_font_1, self.spec.description, size.x-5-Theme.width_icon_1)
		local h = (h1 or 0) + (h2 or 0)
		size.y = math.max(size.y, h + 10)
		size.y = math.max(size.y, Theme.width_icon_1+10)
		self.title_height = h1 or 0
	end
	return size
end

Widgets.Uispell.rebuild_canvas = function(self)
	local w = self.size.x
	local h = self.size.y
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the icon.
	-- Add the icon.
	if self.spec then
		Theme:draw_icon_scaled(self, self.spec.icon,
			5, 5, Theme.width_icon_1, Theme.width_icon_1,
			self.active and {1,1,1,1} or {0.3,0.3,0.3,0.3})
	end
	-- Add the name.
	if self.spec then
		local cap = self:get_pretty_name()
		self:canvas_text{
			dest_position = {5+Theme.width_icon_1,5},
			dest_size = {w-5-Theme.width_icon_1,h},
			text = cap,
			text_alignment = {0,0},
			text_color = self.active and Theme.text_color_1 or Theme.text_color_2,
			text_font = Theme.text_font_2}
	end
	-- Add the description.
	if self.spec and self.title_height then
		self:canvas_text{
			dest_position = {5+Theme.width_icon_1,5+self.title_height},
			dest_size = {w-5-Theme.width_icon_1,h},
			text = self.spec.description,
			text_alignment = {0,0},
			text_color = self.active and Theme.text_color_1 or Theme.text_color_2,
			text_font = Theme.text_font_1}
	end
end

Widgets.Uispell.get_pretty_name = function(self)
	if not self.spec then return end
	return string.gsub(self.spec.name, "(.)(.*)", function(a,b) return string.upper(a) .. b end)
end

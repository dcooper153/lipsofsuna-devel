require(Mod.path .. "widget")

Widgets.Uiskill = Class(Widgets.Uiwidget)
Widgets.Uiskill.class_name = "Widgets.Uiskill"

Widgets.Uiskill.new = function(clss, skill, active, value)
	local self = Widgets.Uiwidget.new(clss)
	self.skill = skill
	self.active = active
	self.value = value
	self.hint = (value and "$A: Disable\n$$B\n$$U\n$$D") or
	            (active and "$A: Enable\n$$B\n$$U\n$$D") or "$$B\n$$U\n$$D"
	return self
end

Widgets.Uiskill.apply = function(self)
	if not self.active then return end
	-- Toggle the value.
	self.value = not self.value
	self.hint = self.value and "$A: Disable\n$$B\n$$U\n$$D" or "$A: Enable\n$$B\n$$U\n$$D"
	self.need_repaint = true
	-- Add or remove the skill.
	if self.value then
		Client.data.skills:add(self.skill.name)
		Effect:play_global("uitoggle1")
	else
		Client.data.skills:remove(self.skill.name)
		Ui:restart_state()
		Effect:play_global("uitoggle2")
	end
	-- Send an update.
	Game.messaging:client_event("update skills", Client.data.skills:get_names())
end

Widgets.Uiskill.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the description.
	if self.skill then
		local w1,h1 = Program:measure_text(Theme.text_font_2, self.skill.name, size.x-5-Theme.width_icon_1)
		local w2,h2 = Program:measure_text(Theme.text_font_1, self.skill.description, size.x-5-Theme.width_icon_1)
		local h = (h1 or 0) + (h2 or 0)
		size.y = math.max(size.y, h + 10)
		size.y = math.max(size.y, Theme.width_icon_1+10)
		self.title_height = h1 or 0
	end
	return size
end

Widgets.Uiskill.rebuild_canvas = function(self)
	local w = self.size.x
	local h = self.size.y
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the icon.
	if self.skill then
		Theme:draw_icon_scaled(self, self.skill.icon or "missing1",
			5, 5, Theme.width_icon_1, Theme.width_icon_1,
			self.value and {1,1,1,1} or {0.3,0.3,0.3,0.3})
	end
	-- Add the name.
	if self.skill then
		self:canvas_text{
			dest_position = {5+Theme.width_icon_1,5},
			dest_size = {w-5-Theme.width_icon_1,h},
			text = self.skill.name,
			text_alignment = {0,0},
			text_color = self.active and Theme.text_color_1 or Theme.text_color_2,
			text_font = Theme.text_font_2}
	end
	-- Add the description.
	if self.skill and self.title_height then
		self:canvas_text{
			dest_position = {5+Theme.width_icon_1,5+self.title_height},
			dest_size = {w-5-Theme.width_icon_1,h},
			text = self.skill.description,
			text_alignment = {0,0},
			text_color = self.active and Theme.text_color_1 or Theme.text_color_2,
			text_font = Theme.text_font_1}
	end
end

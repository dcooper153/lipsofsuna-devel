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
	self.icon = Iconspec:find{name = skill.icon}
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
	else
		Client.data.skills:remove(self.skill.name)
		Ui:restart_state()
	end
	-- Send an update.
	local packet = Packet(packets.PLAYER_SKILLS)
	for k in pairs(Client.data.skills:get_names()) do
		packet:write("string", k)
	end
	Network:send{packet = packet}
end

Widgets.Uiskill.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the description.
	if self.skill then
		local w,h = Program:measure_text("default", self.skill.description, size.x - 40)
		if h then size.y = math.max(size.y, h + 25) end
	end
	return size
end

Widgets.Uiskill.rebuild_canvas = function(self)
	local a = self.active and 1 or 0.5
	local w = self.size.x
	local h = self.size.y
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the selection mark.
	-- FIXME
	self:canvas_image{
		dest_position = {5,3},
		dest_size = {32,self.size.y-6},
--		dest_size = {self.size.x-10,self.size.y-6},
		source_image = "widgets1",
		source_position = self.value and {350,415} or {350,375},
		source_tiling = {12,64,12,11,14,13}}
	-- Add the icon.
	local icon = self.icon
	if icon then
		self:canvas_image{
			dest_position = {5,(h-icon.size[2])/2},
			dest_size = {icon.size[1],icon.size[2]},
			color = {1,1,1,a},
			source_image = icon.image,
			source_position = icon.offset,
			source_tiling = {0,icon.size[1],0,0,icon.size[2],0}}
	end
	-- Add the name.
	if self.skill then
		self:canvas_text{
			dest_position = {40,5},
			dest_size = {w-40,h},
			text = self.skill.name,
			text_alignment = {0,0},
			text_color = {a,a,a,1},
			text_font = "bigger"}
	end
	-- Add the description.
	if self.skill then
		self:canvas_text{
			dest_position = {40,20},
			dest_size = {w-40,h},
			text = self.skill.description,
			text_alignment = {0,0},
			text_color = {a,a,a,1},
			text_font = "default"}
	end
end

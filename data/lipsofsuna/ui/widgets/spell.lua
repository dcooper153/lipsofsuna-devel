require(Mod.path .. "widget")

Widgets.Uispell = Class(Widgets.Uiwidget)
Widgets.Uispell.class_name = "Widgets.Uispell"

Widgets.Uispell.new = function(clss, mode, name, active)
	local self = Widgets.Uiwidget.new(clss)
	if mode == "type" then
		self.spec = Feattypespec:find{name = name}
	else
		self.spec = Feateffectspec:find{name = name}
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
end

Widgets.Uispell.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the description.
	if self.spec then
		local w,h = Program:measure_text("default", self.spec.description, size.x - 40)
		if h then size.y = math.max(size.y, h + 25) end
	end
	return size
end

Widgets.Uispell.rebuild_canvas = function(self)
	local a = self.active and 1 or 0.5
	local w = self.size.x
	local h = self.size.y
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
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
	if self.spec then
		local cap = string.gsub(self.spec.name, "(.)(.*)", function(a,b) return string.upper(a) .. b end)
		self:canvas_text{
			dest_position = {40,5},
			dest_size = {w-40,h},
			text = cap,
			text_alignment = {0,0},
			text_color = {a,a,a,1},
			text_font = "bigger"}
	end
	-- Add the description.
	if self.spec then
		self:canvas_text{
			dest_position = {40,20},
			dest_size = {w-40,h},
			text = self.spec.description,
			text_alignment = {0,0},
			text_color = {a,a,a,1},
			text_font = "default"}
	end
end

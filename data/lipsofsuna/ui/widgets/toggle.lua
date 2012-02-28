require(Mod.path .. "widget")

Widgets.Uitoggle = Class(Widgets.Uiwidget)
Widgets.Uitoggle.class_name = "Widgets.Uitoggle"

Widgets.Uitoggle.new = function(clss, label, changed)
	local self = Widgets.Uiwidget.new(clss, nil, label)
	self.value = false
	self.changed = changed
	self.hint = "$A: Toggle\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uitoggle.apply = function(self)
	self.value = not self.value
	self.need_repaint = true
	self:changed()
end

Widgets.Uitoggle.changed = function(self)
end

Widgets.Uitoggle.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the label.
	if self.text then
		local w,h = Program:measure_text("default", "Yes")
		if h then size.y = math.max(size.y, h + 10) end
	end
	return size
end

Widgets.Uitoggle.rebuild_canvas = function(self)
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the background.
	self:canvas_image{
		dest_position = {150,3},
		dest_size = {self.size.x-155,self.size.y-6},
		source_image = "widgets1",
		source_position = self.value and {350,415} or {350,375},
		source_tiling = {12,64,12,11,14,13}}
	-- Add the text.
	self:canvas_text{
		dest_position = {155,5 + (self.value and 3 or 0)},
		dest_size = {self.size.x-160,self.size.y-10},
		text = self.value and "Yes" or "No",
		text_alignment = {0.5,0.5},
		text_color = {1,1,1,1},
		text_font = "default"}
end

require(Mod.path .. "widget")

Widgets.Uibutton = Class(Widgets.Uiwidget)
Widgets.Uibutton.class_name = "Widgets.Uibutton"

Widgets.Uibutton.new = function(clss, label, apply)
	local self = Widgets.Uiwidget.new(clss)
	self.apply = apply
	self.hint = "$A: Activate\n$$B\n$$U\n$$D"
	self.text = label
	return self
end

Widgets.Uibutton.apply = function(self)
end

Widgets.Uibutton.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the label.
	if self.text then
		local w,h = Program:measure_text("default", self.text, 290)
		if h then size.y = math.max(size.y, h + 10) end
	end
	return size
end

Widgets.Uibutton.rebuild_canvas = function(self)
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the background.
	self:canvas_image{
		dest_position = {5,3},
		dest_size = {self.size.x-10,self.size.y-6},
		source_image = "widgets1",
		source_position = self.focused and {350,415} or {350,375},
		source_tiling = {12,64,12,11,14,13}}
	-- Add the text.
	self:canvas_text{
		dest_position = {5,5 + (self.focused and 3 or 0)},
		dest_size = {self.size.x-10,self.size.y-10},
		text = self.text,
		text_alignment = {0.5,0.5},
		text_color = {1,1,1,1},
		text_font = "default"}
end

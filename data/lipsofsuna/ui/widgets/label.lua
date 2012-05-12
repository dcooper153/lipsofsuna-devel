require(Mod.path .. "widget")

Widgets.Uilabel = Class(Widgets.Uiwidget)
Widgets.Uilabel.class_name = "Widgets.Uilabel"

Widgets.Uilabel.new = function(clss, label)
	local self = Widgets.Uiwidget.new(clss)
	self.apply = apply
	self.hint = "$$B\n$$U\n$$D"
	self.text = label
	return self
end

Widgets.Uilabel.apply = function(self)
end

Widgets.Uilabel.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the label.
	if self.text then
		local w,h = Program:measure_text(Theme.text_font_1, self.text, size.x - 10)
		if h then size.y = math.max(size.y, h + 10) end
	end
	return size
end

Widgets.Uilabel.rebuild_canvas = function(self)
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the text.
	self:canvas_text{
		dest_position = {5,5},
		dest_size = {self.size.x-10,self.size.y-10},
		text = self.text,
		text_alignment = {0,0},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
end

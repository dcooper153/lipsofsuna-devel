local Class = require("system/class")
local UiWidget = require("ui/widgets/widget")

local UiLabel = Class("UiLabel", UiWidget)

UiLabel.new = function(clss, label)
	local self = UiWidget.new(clss)
	self.hint = "$$B\n$$U\n$$D"
	self.text = label
	return self
end

UiLabel.apply = function(self)
end

UiLabel.rebuild_size = function(self)
	-- Get the base size.
	local size = UiWidget.rebuild_size(self)
	-- Resize to fit the label.
	if self.text then
		local w,h = Program:measure_text(Theme.text_font_1, self.text, size.x - 10)
		if h then size.y = math.max(size.y, h + 10) end
	end
	return size
end

UiLabel.rebuild_canvas = function(self)
	-- Add the base.
	UiWidget.rebuild_canvas(self)
	-- Add the text.
	self:canvas_text{
		dest_position = {5,5},
		dest_size = {self.size.x-10,self.size.y-10},
		text = self.text,
		text_alignment = {0,0},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
end

return UiLabel

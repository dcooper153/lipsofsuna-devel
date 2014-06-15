local Class = require("system/class")
local Graphics = require("system/graphics")
local UiRadio = require("ui/widgets/radio")

local UiQuest = Class("Uiquest", UiRadio)

UiQuest.new = function(clss, quest)
	local self = UiRadio.new(clss, nil, "quest")
	self.quest = quest
	if quest.status == "completed" then
		self.title = quest.spec.name .. " (completed)"
		self.text = quest.text
	else
		self.title = quest.spec.name
		self.text = quest.text
	end
	self.hint = "$A: Show in compass\n$$B\n$$U\n$$D"
	return self
end

UiQuest.changed = function(self)
	Operators.quests:set_active_quest(self.quest)
end

UiQuest.rebuild_size = function(self)
	-- Get the base size.
	local size = UiRadio.rebuild_size(self)
	-- Resize to fit the label.
	if self.title and self.text then
		local w1,h1 = Graphics:measure_text(Theme.text_font_2, self.title, size.x - 10)
		local w2,h2 = Graphics:measure_text(Theme.text_font_1, self.text, size.x - 10)
		if h1 and h2 then
			size.y = math.max(size.y, h1 + h2 + 20)
			self.title_height = h1 + 3
		end
	end
	return size
end

UiQuest.rebuild_canvas = function(self)
	-- Add the base.
	UiRadio.rebuild_canvas(self)
	-- Add the background.
	Theme:draw_button(self, nil,
		5, 3, self.size.x-10, self.size.y-6,
		self.focused, self.value)
	-- Add the title.
	self:canvas_text{
		dest_position = {10,5 + (self.value and 3 or 0)},
		dest_size = {self.size.x-20,self.size.y-10},
		text = self.title,
		text_alignment = {0.5,0},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_2}
	-- Add the text.
	local text_y = (self.title_height or 0) + (self.value and 3 or 0)
	self:canvas_text{
		dest_position = {10,5 + text_y},
		dest_size = {self.size.x-20,self.size.y-text_y-10},
		text = self.text,
		text_alignment = {0,0},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
end

return UiQuest

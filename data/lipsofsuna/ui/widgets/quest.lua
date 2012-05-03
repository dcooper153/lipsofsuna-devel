require(Mod.path .. "radio")

Widgets.Uiquest = Class(Widgets.Uiradio)
Widgets.Uiquest.class_name = "Widgets.Uiquest"

Widgets.Uiquest.new = function(clss, quest)
	local self = Widgets.Uiradio.new(clss, nil, "quest")
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

Widgets.Uiquest.changed = function(self)
	Operators.quests:set_active_quest(self.quest)
end

Widgets.Uiquest.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the label.
	if self.title and self.text then
		local w1,h1 = Program:measure_text("bigger", self.title, 270)
		local w2,h2 = Program:measure_text("default", self.text, 270)
		if h1 and h2 then
			size.y = math.max(size.y, h1 + h2 + 20)
			self.title_height = h1 + 3
		end
	end
	return size
end

Widgets.Uiquest.rebuild_canvas = function(self)
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the background.
	self:canvas_image{
		dest_position = {5,3},
		dest_size = {self.size.x-10,self.size.y-6},
		source_image = "widgets1",
		source_position = self.value and {350,415} or {350,375},
		source_tiling = {12,64,12,11,14,13}}
	-- Add the title.
	self:canvas_text{
		dest_position = {10,5 + (self.value and 3 or 0)},
		dest_size = {self.size.x-20,self.size.y-10},
		text = self.title,
		text_alignment = {0.5,0},
		text_color = {1,1,1,1},
		text_font = "bigger"}
	-- Add the text.
	local text_y = (self.title_height or 0) + (self.value and 3 or 0)
	self:canvas_text{
		dest_position = {10,5 + text_y},
		dest_size = {self.size.x-20,self.size.y-text_y-10},
		text = self.text,
		text_alignment = {0,0},
		text_color = {1,1,1,1},
		text_font = "default"}
end

local Class = require("system/class")
require(Mod.path .. "widget")

Widgets.Uidialogtext = Class("Uidialogtext", Widgets.Uiwidget)

Widgets.Uidialogtext.new = function(clss, id, index, char, text)
	local self = Widgets.Uiwidget.new(clss)
	self.dialog_id = id
	self.dialog_index = index
	self.char = char
	self.text = text
	self.hint = "$A: Next line\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uidialogtext.apply = function(self)
	Main.messaging:client_event("choose dialog answer", self.dialog_id, "")
	Client.effects:play_global("uitransition1")
end

Widgets.Uidialogtext.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the label.
	if self.text and self.char then
		local w1,h1 = Program:measure_text(Theme.text_font_1, self.char, size.x - 25)
		local w2,h2 = Program:measure_text(Theme.text_font_1, self.text, size.x - 10)
		if h1 and h2 then size.y = math.max(size.y, h1 + h2 + 15) end
		self.title_width = w1
		self.title_height = math.max(20, (h1 or 0) + 5)
	end
	return size
end

Widgets.Uidialogtext.rebuild_canvas = function(self)
	if not self.title_height then return end
	-- Add the backgrounds.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,self.title_height},
		dest_size = {self.size.x, math.max(80, self.size.y-self.title_height)},
		source_image = "widgets3",
		source_position = {0,480},
		source_tiling = {-1,600,-1,-1,80,-1}}
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {self.size.x, self.title_height},
		source_image = "widgets3",
		source_position = {0,440},
		source_tiling = {-1,600,-1,-1,40,-1}}
	-- Add the character name.
	self:canvas_text{
		dest_position = {20,5},
		dest_size = {self.size.x-25,self.size.y-10},
		text = self.char,
		text_alignment = {0,0},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
	-- Add the text.
	self:canvas_text{
		dest_position = {5,self.title_height + 10},
		dest_size = {self.size.x-10,self.size.y-self.title_height-20},
		text = self.text,
		text_alignment = {0,0},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
end

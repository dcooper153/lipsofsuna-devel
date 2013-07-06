local Class = require("system/class")
require(Mod.path .. "label")

Widgets.Uidialogchoice = Class("Uidialogchoice", Widgets.Uilabel)

Widgets.Uidialogchoice.new = function(clss, id, index, text)
	local self = Widgets.Uilabel.new(clss, text)
	self.dialog_id = id
	self.dialog_index = index
	self.hint = "$A: Reply\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uidialogchoice.apply = function(self)
	Main.messaging:client_event("choose dialog answer", self.dialog_id, self.text)
	Client.effects:play_global("transition1")
end

Widgets.Uidialogchoice.rebuild_canvas = function(self)
	-- Add the background.
	Theme:draw_button(self, nil,
		0, 0, self.size.x, self.size.y,
		self.focused, false)
	-- Add the text.
	self:canvas_text{
		dest_position = {5,5},
		dest_size = {self.size.x-10,self.size.y-10},
		text = self.text,
		text_alignment = {0,0},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
end

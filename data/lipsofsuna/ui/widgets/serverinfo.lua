local Class = require("system/class")
local UiWidget = require("ui/widgets/widget")

local UiServerInfo = Class("Uiserverinfo", UiWidget)

UiServerInfo.new = function(clss, args)
	local self = UiWidget.new(clss)
	for k,v in pairs(args) do self[k] = v end
	self.hint = "$A: Select\n$$B\n$$U\n$$D"
	return self
end

UiServerInfo.apply = function(self)
	Client.options.join_address = self.ip
	Client.options.join_port = self.port
	Ui:pop_state()
end

UiServerInfo.rebuild_size = function(self)
	-- Get the base size.
	local size = UiWidget.rebuild_size(self)
	-- Resize to fit the label.
	if self.desc then
		local w1,h1 = Program:measure_text(Theme.text_font_2, self.name, 290)
		local w2,h2 = Program:measure_text(Theme.text_font_1, self.desc, 290)
		if h1 and h2 then size.y = math.max(size.y, h1 + h2 + 30) end
		self.title_height = h1
	end
	return size
end

UiServerInfo.rebuild_canvas = function(self)
	local a = 1
	local w = self.size.x
	local h = self.size.y
	-- Add the base.
	UiWidget.rebuild_canvas(self)
	-- Add the name.
	if self.name then
		self:canvas_text{
			dest_position = {10,5},
			dest_size = {w-10,h},
			text = self.name,
			text_alignment = {0,0},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_2}
	end
	-- Add the description.
	if self.desc and self.title_height then
		self:canvas_text{
			dest_position = {10,5+self.title_height},
			dest_size = {w-10,h},
			text = self.desc,
			text_alignment = {0,0},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_1}
	end
	-- Add the player count.
	if self.players then
		self:canvas_text{
			dest_position = {0,8},
			dest_size = {w-10,h},
			text = tostring(self.players),
			text_alignment = {1,0},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_1}
	end
end

return UiServerInfo

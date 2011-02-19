require "client/widgets/label"

Widgets.Entry = Class(Widgets.Label)

Widgets.Entry.new = function(clss, args)
	local self = Widgets.Label.new(clss, args)
	self.init = true
	return self
end

Widgets.Entry.clear = function(self, args)
	self.text = ""
end

--- Handles input.
-- @param self Entry widget.
-- @param args Arguments.
-- @return True if absorbed.
Widgets.Entry.event = function(self, args)
	if args.type == "keypress" then
		if args.code == 8 then
			-- Backspace.
			if self.text and #self.text then
				self.text = string.sub(self.text, 1, math.max(0, #self.text - 1))
			end
		elseif args.code == 13 then
			-- Enter.
			if self.pressed then
				self:pressed()
			end
		elseif args.text then
			-- Typing.
			self.text = (self.text or "") .. args.text
		end
		return true
	end
end

Widgets.Entry.reshaped = function(self)
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = (self.text or "") .. "\n"}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	if not self.transparent then
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = self.focused and {967,383} or {910,383},
			source_tiling = {15,25,15,10,14,10}}
	end
	self:canvas_text{
		dest_position = {2,0},
		dest_size = {w,h},
		text = self.text and (self.text .. (self.focused and "|" or "")),
		text_alignment = {0,0.5},
		text_color = self.focused and {1,1,1,1} or {0.6,0.6,0.6,1},
		text_font = self.font}
	self:canvas_compile()
end

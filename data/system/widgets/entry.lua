require "system/string"
require "system/widgets/label"

Widgets.Entry = Class(Widgets.Label)
Widgets.Entry.class_name = "Widgets.Entry"

--- Creates a new text entry widget.
-- @param clss Entry class.
-- @param args Arguments.
-- @return Entry widget.
Widgets.Entry.new = function(clss, args)
	local self = Widgets.Label.new(clss, args)
	if not self.cursor_pos then self.cursor_pos = 1 end
	self.background = args and args.background
	self.background_focus = args and args.background_focus
	if not self.background then
		self.background = {
			dest_position = {0,0},
			source_image = "widgets1",
			source_position = {910,383},
			source_tiling = {15,25,15,10,14,10}}
		self.background_focus = {
			dest_position = {0,0},
			source_image = "widgets1",
			source_position = {967,383},
			source_tiling = {15,25,15,10,14,10}}
	end
	self.init = true
	return self
end

--- Clears the text of the entry.
-- @param self Entry widget.
Widgets.Entry.clear = function(self)
	self.text = ""
end

--- Handles input.
-- @param self Entry widget.
-- @param args Arguments.
-- @return True if absorbed.
Widgets.Entry.event = function(self, args)
	if args.type == "keypress" then
		if args.code == Keysym.BACKSPACE then
			-- Backspace.
			if self.cursor_pos > 1 then
				local w = String.utf8_to_wchar(self.text)
				local p = self.cursor_pos - 1
				table.remove(w, p)
				self.text = String.wchar_to_utf8(w)
				self.cursor_pos = p
			end
		elseif args.code == Keysym.DELETE then
			-- Delete.
			local w = String.utf8_to_wchar(self.text)
			local p = self.cursor_pos
			if p <= #w then
				table.remove(w, p)
				self.text = String.wchar_to_utf8(w)
				self.cursor_pos = p
			end
		elseif args.code == Keysym.RETURN then
			-- Enter.
			if self.pressed then
				self:pressed()
			end
		elseif args.code == Keysym.LEFT then
			-- Move left.
			if self.cursor_pos > 1 then
				self.cursor_pos = self.cursor_pos - 1
			end
		elseif args.code == Keysym.RIGHT then
			-- Move right.
			local w = String.utf8_to_wchar(self.text)
			if self.cursor_pos <= #w then
				self.cursor_pos = self.cursor_pos + 1
			end
		elseif args.text then
			-- Typing.
			local w = String.utf8_to_wchar(self.text or "")
			local p = self.cursor_pos
			local i = String.utf8_to_wchar(args.text)
			for k,v in ipairs(i) do
				table.insert(w, p, v)
				p = p + 1
			end
			self.text = String.wchar_to_utf8(w)
			self.cursor_pos = p
		end
		return true
	end
end

--- Rebuilds the entry widget.
-- @param self Entry widget.
Widgets.Entry.reshaped = function(self)
	-- Format the text.
	-- In password mode, all characters are replaced with '*'.
	local text
	if self.password and self.text then
		text = ""
		for i=1,#self.text do text = text .. "*" end
	else
		text = self.text or ""
	end
	-- Calculate the size request.
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = text .. "\n"}
	local w = self.width
	local h = self.height
	-- Pack the background.
	self:canvas_clear()
	local style = self.focus and self.background_focus or self.background
	if style then
		self:canvas_image{
			dest_position = style.dest_position,
			dest_size = {w,h},
			source_image = style.source_image,
			source_position = style.source_position,
			source_tiling = style.source_tiling}
	end
	-- Pack the text.
	self:canvas_text{
		dest_position = {2,0},
		dest_size = {w,h},
		text = text,
		text_alignment = {0,0.5},
		text_color = self.color or self.focused and {1,1,1,1} or {0.6,0.6,0.6,1},
		text_font = self.font}
	-- Pack the cursor.
	if self.focused then
		local ws = String.utf8_to_wchar(text)
		ws[self.cursor_pos] = nil
		local us = String.wchar_to_utf8(ws)
		local pos = Program:measure_text(self.font, us, w)
		self:canvas_text{
			dest_position = {2+pos,0},
			dest_size = {w,h},
			text = "|",
			text_alignment = {0,0.5},
			text_color = self.color or {1,1,1,1},
			text_font = self.font}
	end
	self:canvas_compile()
end

Widgets.Entry:add_getters{
	cursor_pos = function(s) return rawget(s, "__cursor_pos") end,
	password = function(s) return rawget(s, "__password") end}

Widgets.Entry:add_setters{
	cursor_pos = function(s, v)
		rawset(s, "__cursor_pos", v)
		s:reshaped()
	end,
	password = function(s, v)
		if s.password == v then return end
		rawset(s, "__password", v and true or nil)
		s:reshaped()
	end,
	text = function(s, v)
		if s.text == v then return end
		if type(v) == "string" then
			rawset(s, "__text", v)
		else
			rawset(s, "__text", tostring(v))
		end
		if s.text then
			local w = String.utf8_to_wchar(s.text)
			rawset(s, "__cursor_pos", #w + 1)
		else
			rawset(s, "__cursor_pos", 1)
		end
		s:reshaped()
	end}

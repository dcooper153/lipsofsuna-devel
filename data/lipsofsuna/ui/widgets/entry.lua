require(Mod.path .. "widget")

Widgets.Uientry = Class(Widgets.Uiwidget)
Widgets.Uientry.class_name = "Widgets.Uientry"

Widgets.Uientry.new = function(clss, label, changed)
	local self = Widgets.Uiwidget.new(clss, nil, label)
	self.value = ""
	self.cursor_pos = 1
	self.changed = changed
	self.hint = "$A: Edit\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uientry.accepted = function(self)
end

Widgets.Uientry.canceled = function(self)
end

Widgets.Uientry.apply = function(self)
	self.input_mode = true
	self.hint = "[RETURN]: End editing"
	self.need_repaint = true
end

Widgets.Uientry.changed = function(self)
end

Widgets.Uientry.handle_event = function(self, args)
	if not self.input_mode or args.type ~= "keypress" then
		return Widgets.Uiwidget.handle_event(self, args)
	end
	if args.code == Keysym.BACKSPACE then
		-- Backspace.
		if self.cursor_pos > 1 then
			local w = String.utf8_to_wchar(self.value)
			local p = self.cursor_pos - 1
			table.remove(w, p)
			self.value = String.wchar_to_utf8(w)
			self.cursor_pos = p
			self.need_repaint = true
			self:changed()
		end
	elseif args.code == Keysym.DELETE then
		-- Delete.
		local w = String.utf8_to_wchar(self.value)
		local p = self.cursor_pos
		if p <= #w then
			table.remove(w, p)
			self.value = String.wchar_to_utf8(w)
			self.cursor_pos = p
			self.need_repaint = true
			self:changed()
		end
	elseif args.code == Keysym.RETURN or args.code == Keysym.ESCAPE then
		-- Enter / escape.
		self.input_mode = nil
		self.hint = "$A: Edit\n$$B\n$$U\n$$D"
		self.need_repaint = true
		if args.code == Keysym.RETURN then
			self:accepted()
		else
			self:canceled()
		end
	elseif args.code == Keysym.LEFT then
		-- Move left.
		if self.cursor_pos > 1 then
			self.cursor_pos = self.cursor_pos - 1
			self.need_repaint = true
		end
	elseif args.code == Keysym.RIGHT then
		-- Move right.
		local w = String.utf8_to_wchar(self.value)
		if self.cursor_pos <= #w then
			self.cursor_pos = self.cursor_pos + 1
			self.need_repaint = true
		end
	elseif args.text then
		-- Typing.
		local w = String.utf8_to_wchar(self.value or "")
		local p = self.cursor_pos
		local i = String.utf8_to_wchar(args.text)
		for k,v in ipairs(i) do
			table.insert(w, p, v)
			p = p + 1
		end
		self.value = String.wchar_to_utf8(w)
		self.cursor_pos = p
		self.need_repaint = true
		self:changed()
	end
end

Widgets.Uientry.rebuild_canvas = function(self)
	-- Format the text.
	-- In password mode, all characters are replaced with '*'.
	local text
	if self.password and self.value then
		text = ""
		for i=1,#self.value do text = text .. "*" end
	else
		text = self.value
	end
	-- Add the base.
	local w = self.size.x - 159
	local h = self.size.y - 10
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the background.
	self:canvas_image{
		dest_position = {150,5},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = self.focused and {967,383} or {910,383},
		source_tiling = {15,25,15,10,14,10}}
	-- Add the text.
	self:canvas_text{
		dest_position = {152,5},
		dest_size = {w,h},
		text = text,
		text_alignment = {0,0.5},
		text_color = {1,1,1,1},
		text_font = "default"}
	-- Add the cursor.
	if self.input_mode then
		local ws = String.utf8_to_wchar(text)
		ws[self.cursor_pos] = nil
		local us = String.wchar_to_utf8(ws)
		local pos = Program:measure_text("default", us, w)
		self:canvas_text{
			dest_position = {152+pos,5},
			dest_size = {w,h},
			text = "|",
			text_alignment = {0,0.5},
			text_color = {1,1,1,1},
			text_font = "default"}
	end
	self:canvas_compile()
end

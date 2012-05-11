require(Mod.path .. "widget")

Widgets.Uientry = Class(Widgets.Uiwidget)
Widgets.Uientry.class_name = "Widgets.Uientry"

Widgets.Uientry.new = function(clss, label, changed)
	local self = Widgets.Uiwidget.new(clss, nil, label)
	self.value = ""
	self.cursor_pos = 1
	self.changed = changed
	self.hint = "$A: Edit\n$$B\n$$U\n$$D"
	self.need_reshape = true
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
	if not self.input_mode or (args.type ~= "keypress" and args.type ~= "keyrepeat") then
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
			self.need_reshape = true
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
			self.need_reshape = true
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
	elseif args.code == Keysym.UP then
		-- Move up.
		self:move_cursor_up()
		self.need_repaint = true
	elseif args.code == Keysym.DOWN then
		-- Move down.
		self:move_cursor_down()
		self.need_repaint = true
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
		self.need_reshape = true
		self.need_repaint = true
		self:changed()
	end
end

--- Moves the cursor down one line.
-- @param self Entry widget.
Widgets.Uientry.move_cursor_down = function(self)
	-- Layout the text.
	local text = self:get_displayed_text()
	local layout = Program:layout_text(Theme.text_font_1, text, self:get_text_area_width())
	if not layout then return end
	-- Find the closest glyph below the cursor.
	local best
	local cx,cy = self:get_cursor_position()
	for i = 1,#layout,3 do
		local x = layout[i]
		local y = layout[i + 1]
		if y > cy then
			local dist = math.abs(x - cx) + math.abs(y - cy)
			if not best or dist < best then
				self.cursor_pos = (i+2) / 3
				best = dist
			end
		end
	end
end

--- Moves the cursor down one line.
-- @param self Entry widget.
Widgets.Uientry.move_cursor_up = function(self)
	-- Layout the text.
	local text = self:get_displayed_text()
	local layout = Program:layout_text(Theme.text_font_1, text, self:get_text_area_width())
	if not layout then return end
	-- Find the closest glyph above the cursor.
	local best
	local cx,cy = self:get_cursor_position()
	for i = 1,#layout,3 do
		local x = layout[i]
		local y = layout[i + 1]
		if y < cy then
			local dist = math.abs(x - cx) + math.abs(y - cy)
			if not best or dist < best then
				self.cursor_pos = (i+2) / 3
				best = dist
			end
		end
	end
end

Widgets.Uientry.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the label.
	if self.value then
		local text = self:get_displayed_text()
		local w,h = Program:measure_text(Theme.text_font_1, text, self:get_text_area_width(size))
		if h then size.y = math.max(size.y, h + 10) end
	end
	return size
end

Widgets.Uientry.rebuild_canvas = function(self)
	-- Format the text.
	local text = self:get_displayed_text()
	-- Add the base.
	local w = self.size.x - Theme.width_label_1 - 5
	local w1 = self:get_text_area_width()
	local h = self.size.y - 10
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the background.
	Theme:draw_entry(self,
		Theme.width_label_1, 5, w, h,
		self.focused, self.input_mode)
	-- Add the text.
	self:canvas_text{
		dest_position = {Theme.width_label_1+2,5+Theme.text_pad_1},
		dest_size = {w1,h},
		text = text,
		text_alignment = {0,0.5},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
	-- Add the cursor.
	if self.input_mode then
		local cx,cy = self:get_cursor_position()
		self:canvas_text{
			dest_position = {Theme.width_label_1+2+cx,5+Theme.text_pad_1+cy},
			dest_size = {w1,h},
			text = "|",
			text_alignment = {0,0},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_1}
	end
	self:canvas_compile()
end

--- Gets the visual position of the cursor relative to the text area origin.
-- @param self Entry widget.
-- @return X,Y.
Widgets.Uientry.get_cursor_position = function(self)
	local text = self:get_displayed_text()
	local layout = Program:layout_text(Theme.text_font_1, text, self:get_text_area_width())
	if not layout then return 0,0 end
	local cx,cy
	local cp = 3*self.cursor_pos-2
	if cp < #layout then
		cx = layout[cp] or 0
		cy = layout[cp + 1] or 0
	else
		cp = #layout - 2
		cx = (layout[cp] or 0) + (layout[cp + 2] or 0)
		cy = layout[cp + 1] or 0
	end
	return cx,cy
end

--- Gets the characters visible to the user.
-- @param self Entry widget.
-- @return String.
Widgets.Uientry.get_displayed_text = function(self)
	-- In password mode, all characters are replaced with '*'.
	if self.password and self.value then
		local ws = String.utf8_to_wchar(self.value)
		for k,v in pairs(ws) do ws[k] = "*" end
		return string.concat(ws)
	else
		return self.value
	end
end

--- Gets the width of the text area.
-- @param self Entry widget.
-- @return Width in pixels.
Widgets.Uientry.get_text_area_width = function(self, size)
	local s = size or self.size
	return s.x - Theme.width_label_1 - 15
end

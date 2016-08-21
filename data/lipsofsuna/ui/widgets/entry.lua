local Class = require("system/class")
local Graphics = require("system/graphics")
local Keysym = require("system/keysym")
local Keymods = require("system/keymods")
local String = require("system/string")
local UiWidget = require("ui/widgets/widget")
local Clipboard = require("system/clipboard")

local UiEntry = Class("UiEntry", UiWidget)

UiEntry.new = function(clss, label, changed)
	local self = UiWidget.new(clss, label)
	self.value = ""
	self.cursor_pos = 1
	self.changed = changed
	self.hint = "$A: Edit\n$$B\n$$U\n$$D"
	self.need_reshape = true
	self.history_enabled = false
	self.history = {}
	self.history_index = 1
	return self
end

UiEntry.accepted = function(self)
end

UiEntry.canceled = function(self)
end

UiEntry.apply = function(self)
	self.input_mode = true
	self.hint = "[RETURN]: End editing"
	self.need_repaint = true
	Main.effect_manager:play_global("uitoggle1")
end

UiEntry.changed = function(self)
end

UiEntry.handle_event = function(self, args)
	if not self.input_mode or (args.type ~= "keypress" and args.type ~= "keyrepeat") then
		return UiWidget.handle_event(self, args)
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
		if ((math.floor(args.mods / Keymods.LALT) % 2) + (math.floor(args.mods / Keymods.RALT) % 2)) > 0 then
			if self.history_enabled and (args.text == 'p' or args.text == 'P') then
				self:history_select(self.history_index - 1)
			elseif self.history_enabled and (args.text == 'n' or args.text == 'N') then
				self:history_select(self.history_index + 1)
			end
		elseif args.text == '\x16' then --ctrl v
			self:insert_text(Clipboard.get())
		elseif args.text == '\x03' then --ctrl c
			Clipboard.set(self.value)
		else
			self:insert_text(args.text)
		end
	end
end

--- Enables or disables history.
-- @param en Disables history if false, enables history otherwise.
UiEntry.history_enable = function(self, en)
	if en == nil then
		self.history_enabled = true
	else
		self.history_enabled = en and true or false
	end
end

--- Adds a text string to the history list.
-- @param text The text string to append to the end of the history list.
UiEntry.history_add = function(self, text)
	--Don't store if the last one was the same as this.
	if #self.history > 0 and self.history[#self.history] == text then
		return
	end
	--todo: limit length of history
	--todo: remove duplicates?
	self.history[#self.history + 1] = text
	self.history_index = #self.history + 1
end

--- Sets the text of the widget to the history element at the given index, or blank if past the end of the history.
--@param index The index of the history element to use.
UiEntry.history_select = function(self, index)
	self.history_index = index
	if self.history_index > #self.history + 1 then
		self.history_index = #self.history + 1
	end
	if self.history_index < 1 then
		self.history_index = 1
	end
	if self.history_index > #self.history then
		self.value = ""
	else
		self.value = self.history[self.history_index]
	end
	--todo: set cursor position to the end
	self.cursor_pos = 1
	self.need_reshape = true
	self.need_repaint = true
	self:changed()
end

--- Inserts text into the entry widget.
-- @param text The text to insert.
-- @param pos The position to insert to, defaults to cursor_pos.
-- @param move Move cursor_pos after insert, defaults to false if pos supplied or true otherwise.
UiEntry.insert_text = function(self, text, pos, move)
	local w = String.utf8_to_wchar(self.value or "")
	local p = pos or self.cursor_pos
	local i = String.utf8_to_wchar(text)
	for k,v in ipairs(i) do
		table.insert(w, p, v)
		p = p + 1
	end
	self.value = String.wchar_to_utf8(w)
	if move or not pos then
		self.cursor_pos = p
	end
	self.need_reshape = true
	self.need_repaint = true
	self:changed()
end

--- Moves the cursor down one line.
-- @param self Entry widget.
UiEntry.move_cursor_down = function(self)
	-- Layout the text.
	local text = self:get_displayed_text()
	local layout = Graphics:layout_text(Theme.text_font_1, text, self:get_text_area_width())
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
UiEntry.move_cursor_up = function(self)
	-- Layout the text.
	local text = self:get_displayed_text()
	local layout = Graphics:layout_text(Theme.text_font_1, text, self:get_text_area_width())
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

UiEntry.rebuild_size = function(self)
	-- Get the base size.
	local size = UiWidget.rebuild_size(self)
	-- Resize to fit the label.
	if self.value then
		local text = self:get_displayed_text()
		local w,h = Graphics:measure_text(Theme.text_font_1, text, self:get_text_area_width(size))
		if h then size.y = math.max(size.y, h + 10) end
	end
	return size
end

UiEntry.rebuild_canvas = function(self)
	-- Format the text.
	local text = self:get_displayed_text()
	-- Add the base.
	local w = self.size.x - Theme.width_label_1 - 5
	local w1 = self:get_text_area_width()
	local h = self.size.y - 10
	UiWidget.rebuild_canvas(self)
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
UiEntry.get_cursor_position = function(self)
	local text = self:get_displayed_text()
	local layout = Graphics:layout_text(Theme.text_font_1, text, self:get_text_area_width())
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
UiEntry.get_displayed_text = function(self)
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
UiEntry.get_text_area_width = function(self, size)
	local s = size or self.size
	return s.x - Theme.width_label_1 - 15
end

return UiEntry


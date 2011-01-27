require "client/widgets/button"
require "client/widgets/popup"

Widgets.ComboBox = Class(Widgets.Button)

Widgets.ComboBox.new = function(clss, args)
	local self = Widgets.Button.new(clss)
	self.value = 1
	self.menu = Widgets.Popup{cols = 1, temporary = true}
	self.menu:set_expand{col = 1}
	-- Copy arguments.
	for k,v in pairs(args or {}) do
		if type(k) ~= "number" then
			self[k] = v
		end
	end
	-- Create menu items.
	for k,v in ipairs(args or {}) do
		if type(v) == "table" then
			self:append{text = v[1], pressed = v[2]}
		elseif type(v) == "string" then
			self:append{text = v}
		end
	end
	return self
end

Widgets.ComboBox.append = function(self, args)
	local row = self.menu.rows + 1
	local widget = Widgets.MenuItem{text = args.text, pressed = function()
		self.text = args.text
		self.value = row
		self.menu.visible = false
		if args.pressed then args.pressed(self, row) end
	end}
	self.menu:append_row(widget)
end

--- Activates a row of the combo box menu.
-- @param self Combo box.
-- @param args Arguments.<ul>
--   <li>index: Row index.</li>
--   <li>press: False to not call the menu activation function.</li>
--   <li>text: Row text.</li></ul>
-- @return True on success.
Widgets.ComboBox.activate = function(self, args)
	if args.index then
		-- Activate by index.
		local widget = self.menu:get_child{col = 1, row = args.index}
		if not widget then return end
		if args.press ~= false then widget:pressed() end
	elseif args.text then
		-- Activate by text.
		local row = 1
		local widget
		for row = 1,self.menu.rows do
			widget = self.menu:get_child{col = 1, row = row}
			if not widget then return end
			if widget.text == args.text then break end
		end
		if args.press ~= false then widget:pressed() end
	end
	return true
end

Widgets.ComboBox.pressed = function(self)
	if self.menu.rows == 0 then return end
	local p = Client.cursor_pos
	if p.x - self.x < 10 then
		-- Previous item.
		if self.value > 1 then
			self:activate{index = self.value - 1}
		else
			self:activate{index = self.menu.rows} 
		end
	elseif p.x - self.x > self.width - 11 then
		-- Next item.
		if self.value < self.menu.rows then
			self:activate{index = self.value + 1}
		else
			self:activate{index = 1} 
		end
	else
		-- Popup menu.
		self.menu:set_request{width = self.width - 4}
		self.menu:popup{x = self.x, y = self.y, width = self.width, height = self.height, dir = "down"}
	end
end

Widgets.ComboBox.reshaped = function(self)
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,12,12,2},
		text = self.text}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = self.focused and {64,0} or {0,0},
		source_tiling = {6,52,6,6,52,6}}
	self:canvas_text{
		dest_position = {13,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {0,0.5},
		text_color = self.focused and {1,1,0,1} or {1,1,1,1},
		text_font = self.font}
	self:canvas_compile()
end

require "client/widgets/button"
require "client/widgets/frame"

Widgets.ComboBox = Class(Widgets.Button)

Widgets.ComboBox.new = function(clss, args)
	local self = Widgets.Button.new(clss)
	self.value = 1
	self.menu = Widgets.Frame{cols = 1, style = "popup", temporary = true}
	self.menu:set_expand{col = 1}
	-- Copy arguments.
	for k,v in pairs(args or {}) do
		if type(k) ~= "number" then
			self[k] = v
		end
	end
	-- Create menu items.
	self:set_items(args or {})
	return self
end

--- Activates a row of the combo box menu.
-- @param self Combo box.
-- @param args Arguments.<ul>
--   <li>index: Row index.</li>
--   <li>press: False to not call the menu activation function.</li>
--   <li>text: Row text.</li></ul>
-- @return True on success.
Widgets.ComboBox.activate = function(self, args)
	-- Find the item.
	local widget = nil
	local index = nil
	if args.index then
		widget = self.menu:get_child{col = 1, row = args.index}
		index = args.index
	elseif args.text then
		index = 1
		for index = 1,self.menu.rows do
			local tmp = self.menu:get_child{col = 1, row = index}
			if not tmp then return end
			if tmp.text == args.text then
				widget = tmp
				break
			end
		end
	end
	-- Set the selection.
	if not widget then return end
	if args.press ~= false then
		widget:pressed()
	else
		self.active = index
		self.text = widget.text
	end
	return true
end

--- Appends a new item to the combo box.
-- @param self Combo box.
-- @param args Arguments.<ul>
--   <li>pressed: Selection handling function.</li>
--   <li>text: Text of the item.</li></ul>
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

Widgets.ComboBox.clear = function(self)
	self.value = 1
	self.menu.rows = 0
	self.text = nil
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
		self.menu:set_request{width = self.width + 9}
		self.menu:popup{x = self.x, y = self.y, width = self.width, height = self.height, dir = "down"}
	end
end

Widgets.ComboBox.set_items = function(self, items)
	self:clear()
	for k,v in ipairs(items) do
		if type(v) == "table" then
			self:append{text = v[1], pressed = v[2]}
		elseif type(v) == "string" then
			self:append{text = v}
		end
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
	-- Background.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = self.focused and {350,415} or {350,375},
		source_tiling = {12,64,12,11,14,13}}
	-- Arrows.
	self:canvas_image{
		dest_position = {5,(h-14)/2},
		dest_size = {7,14},
		source_image = "widgets1",
		source_position = {651,417},
		source_tiling = {0,7,0,0,14,0}}
	self:canvas_image{
		dest_position = {w-11,(h-14)/2},
		dest_size = {7,14},
		source_image = "widgets1",
		source_position = {667,417},
		source_tiling = {0,7,0,0,14,0}}
	-- Text.
	self:canvas_text{
		dest_position = {15,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {0,0.5},
		text_color = self.focused and {1,1,0,1} or {1,1,1,1},
		text_font = self.font}
	self:canvas_compile()
end

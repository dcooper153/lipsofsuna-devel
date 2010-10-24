Widgets.List = Class(Group)

Widgets.List.new = function(clss, args)
	local self = Group.new(clss, args)
	self.style = "view"
	self.cols = 1
	self.rows = 0
	self:set_expand{col = 1}
	return self
end

--- Appends an item to the list.
-- @param self List widget.
-- @param args Arguments.<ul>
--   <li>text: Button text or nil.</li>
--   <li>widget: Custom widget or nil.</li></ul>
Widgets.List.append = function(self, args)
	local row = self.rows + 1
	if args.text then
		local button = Widgets.IconButton{
			icon = "skill-icon-none",
			text = args.text,
			pressed = function() self:pressed(row) end}
		self:append_row(button)
	elseif args.widget then
		self:append_row(args.widget)
	end
end

Widgets.List.clear = function(self)
	self.rows = 0
end

Widgets.List.get_row = function(self, args)
	local child = self:get_child{col = 1, row = args.row}
	if not child then return end
	return child.text
end

Widgets.List.pressed = function(self, row)
end

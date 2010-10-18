Widgets.Menus = Class(Group)

Widgets.Menus.new = function(clss, args)
	local self = Group.new(clss, {rows = 2, cols = 1, style = "menus", stack = {}})
	self.button_back = Button{style = "menuitem-label", text = "  Back", pressed = function() self:close() end}
	self:set_child{col = 1, row = 2, widget = self.button_back}
	for k,v in pairs(args or {}) do self[k] = v end
	return self
end

--- Opens a new menu level.
-- @param self Menus.
-- @oaram args Arguments.<ul>
--   <li>level: Activated menu level.</li>
--   <li>widget: Submenu to show.</li></ul>
Widgets.Menus.open = function(self, args)
	-- Close levels if opening from an upper level.
	if #self.stack >= args.level then
		self:close{level = args.level}
	end
	-- Append the new widget.
	table.insert(self.stack, args.widget)
	args.widget.level = #self.stack
	self:set_child{row = 1, col = 1, widget = args.widget}
	-- Make sure we are visible.
	self.visible = true
end

Widgets.Menus.close = function(self, args)
	-- Pop levels from the stack.
	local l = math.max(1, args and args.level or #self.stack)
	while #self.stack >= l do
		self.stack[#self.stack] = nil
	end
	-- Show the topmost level or hide if all levels closed.
	if #self.stack > 0 then
		self:set_child{row = 1, col = 1, widget = self.stack[#self.stack]}
	else
		self.visible = false
	end
end

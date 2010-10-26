Widgets.Menus = Class(Widget)

Widgets.Menus.new = function(clss, args)
	local self = Widget.new(clss, {rows = 3, cols = 1, stack = {}})
	self.button_back = Widgets.MenuItem{font = "medium", text = "Back", pressed = function() self:close() end}
	self:set_child{col = 1, row = 2, widget = Widgets.Label{text = "_________________"}}
	self:set_child{col = 1, row = 3, widget = self.button_back}
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
		self:close{hide = false, level = args.level}
	end
	-- Append the new widget.
	table.insert(self.stack, args.widget)
	args.widget.level = #self.stack
	self:set_child{row = 1, col = 1, widget = args.widget}
	-- Disable player controls when the menu is open.
	if not self.visible then
		Client.moving = false
		controls.EDIT_SELECT.enabled = true
		controls.SHOOT.enabled = false
	end
	-- Make sure we are visible.
	self.visible = true
end

--- Closes menu levels.
-- @param self Menus.
-- @oaram args Arguments.<ul>
--   <li>hide: False to not hide the menu even if it becomes empty.</li>
--   <li>level: Closed menu level or nil to close the topmost.</li></ul>
Widgets.Menus.close = function(self, args)
	local h = not args or args.hide ~= false
	-- Pop levels from the stack.
	local l = math.max(1, args and args.level or #self.stack)
	while #self.stack >= l do
		local w = self.stack[#self.stack]
		if w.close then w:close() end
		self.stack[#self.stack] = nil
	end
	-- Show the topmost level or hide if all levels closed.
	if #self.stack > 0 then
		self:set_child{row = 1, col = 1, widget = self.stack[#self.stack]}
	elseif h then
		self.visible = false
	end
	-- Enable player controls when the menu is closed.
	if h and not self.visible then
		Client.moving = true
		controls.EDIT_SELECT.enabled = false
		controls.SHOOT.enabled = true
	end
end


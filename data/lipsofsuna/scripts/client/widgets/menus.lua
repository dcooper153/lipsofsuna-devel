Widgets.Menus = Class(Group)

Widgets.Menus.new = function(clss, args)
	local self = Group.new(clss, {rows = 1, style = "menus"})
	for k,v in pairs(args or {}) do self[k] = v end
	return self
end

--- Opens a new menu level.
-- @param self Menus.
-- @oaram args Arguments.<ul>
--   <li>level: Activated menu level.</li>
--   <li>widget: Submenu to show.</li></ul>
Widgets.Menus.open = function(self, args)
	-- Close menus if opening from an upper level.
	if self.cols >= args.level then
		self.cols = args.level - 1
	end
	-- Append the new widget.
	self:append_col(args.widget)
	args.widget.level = self.cols
	-- Make sure we are visible.
	self.visible = true
end

Widgets.Menus.close = function(self, args)
	-- Close menu levels.
	if not args or not args.level then
		self.cols = self.cols - 1
	else
		self.cols = args.level - 1
	end
	-- Hide if all levels closed.
	if self.rows == 0 then
		self.visible = false
	end
end

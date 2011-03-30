require "client/widgets/tooltip"

Widgets.Itemtooltip = Class(Widgets.Tooltip)

Widgets.Itemtooltip.new = function(clss, args)
	local self = Widgets.Tooltip.new(clss)
	local spec = args.spec
	if not spec then return self end
	-- Icon and name.
	local title = Widget{cols = 2, rows = 1, spacings = {5,0}}
	local icon = Iconspec:find{name = spec.icon}
	if icon then title:set_child{col = 1, row = 1, widget = Widgets.Icon{icon = icon}} end
	title:set_child{col = 2, row = 1, widget = Widgets.Label{font = "bigger", text = spec.name}}
	self:append(title)
	-- Armor.
	local count = args.count or 1
	if spec.armor_class > 0 then
		self:append("armor: " .. tostring(100 * spec.armor_class))
	end
	-- Count.
	if spec.stacking then
		self:append("count: " .. tostring(count))
	end
	-- Weight.
	self:append_row("weight: " .. (spec.mass * count))
	-- Description.
	if spec.description then
		self:append(spec.description)
	end
	return self
end

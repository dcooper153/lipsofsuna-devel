require "client/widgets/tooltip"

Widgets.Feattooltip = Class(Widgets.Tooltip)

Widgets.Feattooltip.new = function(clss, args)
	local self = Widgets.Tooltip.new(clss)
	local feat = args.feat
	if not feat then return self end
	local info = feat:get_info()
	-- Icon and name.
	local title = Widget{cols = 2, rows = 1, spacings = {5,0}}
	local icon = feat:get_icon()
	if icon then title:set_child{col = 1, row = 1, widget = Widgets.Icon{icon = icon}} end
	title:set_child{col = 2, row = 1, widget = Widgets.Label{font = "bigger", text = feat.animation}}
	self:append(title)
	-- Influences.
	if info.influences then
		local first = true
		for k,v in pairs(info.influences) do
			if first then
				self:append(string.format("Powers:"))
				first = false
			end
			if v < 0 then
				self:append(string.format("  %s: %+d damage", k, -v))
			else
				self:append(string.format("  %s: %+d healing", k, v))
			end
		end
	end
	-- Reagents.
	if info.required_reagents then
		local first = true
		for k,v in pairs(info.required_reagents) do
			if first then
				self:append(string.format("Reagents required:"))
				first = false
			end
			self:append(string.format("  %s: %d", k, v))
		end
	end
	return self
end

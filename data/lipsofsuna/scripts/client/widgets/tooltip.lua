Widgets.Tooltip = Class(Widgets.Frame)

Widgets.Tooltip.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.spacings = {0,0}
	self.style = "tooltip"
	self.cols = 1
	return self
end

Widgets.Tooltip.append = function(self, what)
	if type(what) == "string" then
		local label = Widgets.Label{text = what}
		label:set_request{width = 150}
		self:append_row(label)
	else
		self:append_row(what)
	end
end

Widgets.Tooltip.popup = function(self, point)
	self.floating = true
	self.x = point.x
	self.y = point.y
end

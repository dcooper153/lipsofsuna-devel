Views.Inventory = Class(Widget)
Views.Inventory.mode = "inventory"

--- Creates a new inventory view.
-- @param clss Inventory view class.
-- @return Inventory view.
Views.Inventory.new = function(clss)
	local self = Widget.new(clss, {cols = 2, rows = 2})
	self:set_request{width = 100}
	self:set_child{col = 1, row = 1, widget = Widgets.Label{font = "medium", text = "Inventory"}}
	self:set_child{col = 1, row = 2, widget = Equipment.group}
	self:set_expand{col = 2, row = 2}
	return self
end

Views.Inventory.back = function(self)
	Gui:set_mode("menu")
end

--- Closes the inventory view.
-- @param self Inventory view.
Views.Inventory.close = function(self)
end

--- Enters the inventory view.
-- @param self Inventory view.
Views.Inventory.enter = function(self)
end

--- Assigns the player inventory widget to the view.
-- @param self Inventory view.
-- @param widget Player inventory widget.
Views.Inventory.setup = function(self, widget)
	self:set_child{col = 2, row = 2, widget = widget}
end

Views.Inventory.inst = Views.Inventory()

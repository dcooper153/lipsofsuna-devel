Inventory = Class()
Inventory.group = Group{cols = 1, rows = 1, style = "inventory-list"}
Inventory.group:set_expand{col = 1, row = 1}

function Inventory.setup(self, id, widget)
	self.id = id
	self.items = widget
	self.group:set_child{col = 1, row = 1, widget = widget}
end

function Inventory.show(self)
	Gui.inventory_group.visible = true
end

function Inventory.hide(self)
	Gui.inventory_group.visible = false
end

function Inventory.toggle(self)
	Gui.inventory_group.visible = not Gui.inventory_group.visible
end

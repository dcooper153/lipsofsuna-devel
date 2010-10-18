Inventory = Class()
Inventory.group = Group{cols = 1, rows = 1, style = "inventory-list"}
Inventory.group:set_expand{col = 1, row = 1}

Inventory.setup = function(clss, id, widget)
	clss.id = id
	clss.items = widget
	clss.group:set_child{col = 1, row = 1, widget = widget}
end

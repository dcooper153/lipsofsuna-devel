require "common/inventory"

Inventory.setup = function(clss, id, widget)
	clss.id = id
	clss.items = widget
end

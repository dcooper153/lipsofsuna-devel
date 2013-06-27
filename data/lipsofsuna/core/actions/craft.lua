Actionspec{
	name = "craft alchemy",
	label = "Alchemy",
	start = function(action, device)
		action.object:set_crafting_device(device, "alchemy")
	end}

Actionspec{
	name = "craft metal",
	label = "Blacksmith",
	start = function(action, device)
		action.object:set_crafting_device(device, "metal")
	end}

Actionspec{
	name = "craft wood",
	label = "Woodcraft",
	start = function(action, device)
		action.object:set_crafting_device(device, "wood")
	end}

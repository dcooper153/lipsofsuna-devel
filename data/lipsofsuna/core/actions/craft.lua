Actionspec{
	name = "craft alchemy",
	label = "Alchemy",
	func = function(self, user)
		user:set_crafting_device(self, "alchemy")
	end}

Actionspec{
	name = "craft metal",
	label = "Blacksmith",
	func = function(self, user)
		user:set_crafting_device(self, "metal")
	end}

Actionspec{
	name = "craft wood",
	label = "Woodcraft",
	func = function(self, user)
		user:set_crafting_device(self, "wood")
	end}

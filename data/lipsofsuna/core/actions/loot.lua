Actionspec{
	name = "loot",
	label = "Loot",
	start = function(action, item)
		if not item then return end
		item:loot(action.object)
	end}

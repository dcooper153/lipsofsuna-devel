Actionspec{
	name = "craft spells",
	label = "Craft spells",
	start = function(action, item)
		if not item then return end
		action.object:set_crafting_device(item, "spell")
	end}

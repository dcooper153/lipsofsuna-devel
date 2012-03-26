-- Recovers 30 points of mana.
Actionspec{name = "recovermana", type = "item", func = function(item, user)
	if not user.stats then return end
	local value = user.stats:get_value("willpower")
	if not value then return end
	user.stats:set_value("willpower", value + 30)
	item:subtract(1)
	user.inventory:merge_or_drop_object(Item{spec = Itemspec:find{name = "empty bottle"}})
end}

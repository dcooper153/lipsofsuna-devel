-- Cures diseases.
Actionspec{name = "curedisease", type = "item", func = function(item, user)
	user:remove_modifier("plague")
	item:subtract(1)
	user.inventory:merge_or_drop_object(Item{spec = Itemspec:find{name = "empty bottle"}})
end}

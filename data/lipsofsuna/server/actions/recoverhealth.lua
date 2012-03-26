-- Recovers 30 points of health.
Actionspec{name = "recoverhealth", type = "item", func = function(item, user)
	if not user.stats then return end
	local value = user.stats:get_value("health")
	if not value then return end
	user.stats:set_value("health", value + 30)
	item:subtract(1)
	user.inventory:merge_or_drop_object(Item{spec = Itemspec:find{name = "empty bottle"}})
end}

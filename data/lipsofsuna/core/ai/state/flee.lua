Aistatespec{
	name = "flee",
	calculate = function(self)
		local h = self.object.stats:get_value("health")
		if not h or h < 5 then return end
		-- TODO
		-- return 1
	end,
	update = function(self, secs)
		-- TODO
	end}

Aistatespec{
	name = "search",
	calculate = function(self)
		if self.state ~= "combat" then return end
		for k,v in pairs(self.enemies) do return end
		-- TODO
		-- return 1
	end,
	update = function(self, secs)
		-- TODO
	end}

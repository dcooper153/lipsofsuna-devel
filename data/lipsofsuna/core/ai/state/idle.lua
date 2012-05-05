Aistatespec{
	name = "idle",
	calculate = function(self)
		for k,v in pairs(self.enemies) do return end
		return math.random()
	end,
	update = function(self, secs)
		self.object:set_movement(0)
	end}

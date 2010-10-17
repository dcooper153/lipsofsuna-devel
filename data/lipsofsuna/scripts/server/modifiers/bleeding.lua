--- Bleeding modifier.
-- @param self Modifier.
Modifier:register{name = "bleeding", func = function(self)

	-- Only affect mobiles.
	if not self.object.skills then
		return
	end

	-- Bleed for 10 seconds.
	self.object:say("[Bleeding started]")
	while self.timer < 10.0 do
		local t = coroutine.yield()
		self.timer = self.timer + t
		self.object:damaged(3 * t)
		--TODO: Effect
	end
	self.object:say("[Bleeding stopped]")

end}

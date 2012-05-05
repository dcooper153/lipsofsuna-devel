Aistatespec{
	name = "follow",
	calculate = function(self)
		for k,v in pairs(self.enemies) do return end
		if self.object.summon_owner then return 1 end
	end,
	update = function(self, secs)
		if not self.object.summon_owner then return end
		local target = self.object.summon_owner.position
		-- Turn towards the target.
		local face = self.object:face_point{point = target, secs = secs}
		if face < 0.5 then
			self.object:set_movement(0)
			return
		end
		-- Move towards the target.
		local dist = (target - self.object.position).length
		if dist < 3 then
			self.object:set_movement(0)
		else
			self.object:set_movement(1)
		end
	end}

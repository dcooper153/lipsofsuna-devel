Aistatespec{
	name = "chat",
	calculate = function(self)
		-- Check for an active dialog.
		if not self.object.dialog then return end
		-- Check that no combat is occurring.
		for k,v in pairs(self.enemies) do return end
		-- TODO: Check for a nearby player.
		return 1
	end,
	update = function(self, secs)
		-- Turn towards the dialog partner.
		if self.object.dialog then
			self.object:set_movement(0)
			if self.object.dialog.user then
				self.object:face_point{point = self.object.dialog.user.position, secs = secs}
			end
		end
	end}

require(Mod.path .. "npc")

CamperAi = Class(NpcAi)
Ai.dict_name["camper"] = CamperAi

--- Creates a new camper AI.
-- @param clss CamperAi class.
-- @param object Controlled creature.
-- @return AI.
CamperAi.new = function(clss, object)
	local self = NpcAi.new(clss, object)
	return self
end

--- Analyzes the surrounding terrain and chooses a wander target.
-- @param self AI.
CamperAi.choose_wander_target = function(self)
	local home = self.object.home_point
	if home then
		local dist = (self.object.position - self.object.home_point).length
		if dist > 15 * math.random() then
			self.target = home
			if self:avoid_wander_obstacles(home) then return end
		end
	end
	NpcAi.choose_wander_target(self)
end

--- Updates the AI.
-- @param self AI.
-- @param secs Seconds since the last update.
CamperAi.update = function(self, secs)
	-- Store the home point.
	if not self.object.home_point then
		self.object.home_point = self.object.position
	end
	-- Update the base AI.
	NpcAi.update(self, secs)
end

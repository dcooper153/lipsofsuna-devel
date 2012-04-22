require(Mod.path .. "camper")

AnchoredAi = Class(CamperAi)
Ai.dict_name["anchored"] = AnchoredAi

--- Creates a new anchored AI.
-- @param clss CamperAi class.
-- @param object Controlled creature.
-- @return AI.
AnchoredAi.new = function(clss, object)
	local self = CamperAi.new(clss, object)
	return self
end

--- Analyzes the surrounding terrain and chooses a wander target.
-- @param self AI.
AnchoredAi.choose_wander_target = function(self)
	local home = self.object.home_point
	if home then
		self.target = home
		self:avoid_wander_obstacles(home)
	end
end

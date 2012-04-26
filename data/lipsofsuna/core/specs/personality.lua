require(Mod.path .. "spec")

Personalityspec = Class(Spec)
Personalityspec.type = "personality"
Personalityspec.dict_id = {}
Personalityspec.dict_cat = {}
Personalityspec.dict_name = {}

--- Registers an actor personality.
-- @param clss Personalityspec class.
-- @param args Arguments.<ul>
--   <li>categories: List of categories.</li>
--   <li>name: Personality name.</li>
--   <li>phrases_angered: List of angered phrases.</li>
--   <li>phrases_combat: List of combat phrases.</li>
--   <li>phrases_death: List of death phrases.</li></ul>
-- @return New personality spec.
Personalityspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

--- Gets a random phrase for the given situation.
-- @param self Personality spec.
-- @param type Phrase type: "angered"/"combat"/"death".
Personalityspec.get_phrase = function(self, type)
	local rnd = function(tbl)
		if not tbl then return end
		local len = #tbl
		if len == 0 then return end
		return tbl[math.random(1, len)]
	end
	return rnd(self["phrases_" .. type])
end

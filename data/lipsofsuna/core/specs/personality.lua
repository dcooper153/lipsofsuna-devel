require(Mod.path .. "spec")

Personalityspec = Class(Spec)
Personalityspec.type = "personality"
Personalityspec.dict_id = {}
Personalityspec.dict_cat = {}
Personalityspec.dict_name = {}
Personalityspec.introspect = Introspect{
	name = "Personalityspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "phrases_angered", type = "list", list = {type = "string"}, description = "List of angered phrases."},
		{name = "phrases_combat", type = "list", list = {type = "string"}, description = "List of combat phrases."},
		{name = "phrases_death", type = "list", list = {type = "string"}, description = "List of death phrases."}
	}}

--- Registers an actor personality.
-- @param clss Personalityspec class.
-- @param args Arguments.
-- @return New personality spec.
Personalityspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
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

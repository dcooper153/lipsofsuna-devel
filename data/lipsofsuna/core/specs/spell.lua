local Class = require("system/class")
require(Mod.path .. "spec")

Spellspec = Class("Spellspec", Spec)
Spellspec.type = "spell"
Spellspec.dict_id = {}
Spellspec.dict_cat = {}
Spellspec.dict_name = {}
Spellspec.introspect = Introspect{
	name = "Spellspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "model", type = "string", description = "Model to use for the spell."},
		{name = "particle", type = "string", description = "Particle effect to use for the spell."}
	}}

--- Creates a new spell specification.
-- @param clss Spellspec class.
-- @param args Arguments.<ul>
--   <li>model: </li>
--   <li>name: </li>
--   <li>particle: </li></ul>
-- @return New item specification.
Spellspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

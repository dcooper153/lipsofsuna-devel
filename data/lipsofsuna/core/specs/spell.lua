require(Mod.path .. "spec")

Spellspec = Class(Spec)
Spellspec.type = "spell"
Spellspec.dict_id = {}
Spellspec.dict_cat = {}
Spellspec.dict_name = {}

--- Creates a new spell specification.
-- @param clss Spellspec class.
-- @param args Arguments.<ul>
--   <li>model: Model to use for the spell.</li>
--   <li>name: Name of the spell type.</li>
--   <li>particle: Particle effect to use for the spell.</li></ul>
-- @return New item specification.
Spellspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

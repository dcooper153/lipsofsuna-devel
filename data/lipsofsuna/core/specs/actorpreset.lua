require(Mod.path .. "spec")

Actorpresetspec = Class(Spec)
Actorpresetspec.type = "actorpresetspec"
Actorpresetspec.dict_id = {}
Actorpresetspec.dict_cat = {}
Actorpresetspec.dict_name = {}

--- Registers an actor preset.
-- @param clss Actorpresetspec class.
-- @param args Arguments.<ul>
--   <li>body: Body morph table.</li>
--   <li>eye_color: Eye color.</li>
--   <li>face: Face morph table.</li>
--   <li>hair_color: Hair color.</li>
--   <li>hair_style: Hair style name.</li>
--   <li>skin_color: Skin color.</li>
--   <li>height: Height multiplier.</li>
--   <li>name: Preset name.</li></ul>
-- @return New actor preset spec.
Actorpresetspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

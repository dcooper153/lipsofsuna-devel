require(Mod.path .. "spec")

Staticspec = Class(Spec)
Staticspec.class_name = "Staticspec"
Staticspec.type = "static"
Staticspec.dict_id = {}
Staticspec.dict_cat = {}
Staticspec.dict_name = {}

--- Creates a new static map object specification.
-- @param clss Staticspec class.
-- @param args Arguments.<ul>
--   <li>collision_group: Collision group.</li>
--   <li>collision_mask: Collision mask.</li>
--   <li>model: Model to use for the obstacle.</li>
--   <li>name: Name of the object.</li>
--   <li>position: Position vector.</li>
--   <li>rotation: Rotation quaternion.</li></ul>
-- @return New item specification.
Staticspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	local copy = function(f, d) if self[f] == nil then self[f] = d end end
	copy("collision_group", 0x2000)
	copy("collision_mask", 0xFF)
	copy("position", Vector())
	copy("rotation", Quaternion())
	return self
end

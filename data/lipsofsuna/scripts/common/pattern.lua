require "common/spec"

Pattern = Class(Spec)
Pattern.type = "pattern"
Pattern.dict_id = {}
Pattern.dict_cat = {}
Pattern.dict_name = {}

--- Creates a new map pattern.
-- @param clss Pattern class.
-- @param args Arguments.<ul>
--   <li>creatures: Array of creatures to create.</li>
--   <li>items: Array of items to create.</li>
--   <li>obstacles: Array of obstacles to create.</li>
--   <li>tiles: Array of terrain tiles to create.</li></ul>
-- @return New map pattern.
Pattern.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.creatures = self.creatures or {}
	self.items = self.items or {}
	self.obstacles = self.obstacles or {}
	self.tiles = self.tiles or {}
	return self
end

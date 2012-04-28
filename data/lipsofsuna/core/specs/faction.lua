require(Mod.path .. "spec")

Factionspec = Class(Spec)
Factionspec.type = "faction"
Factionspec.dict_id = {}
Factionspec.dict_cat = {}
Factionspec.dict_name = {}
Factionspec.introspect = Introspect{
	name = "Factionspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "enemies", type = "dict", dict = {type = "boolean"}, default = {}, description = "List of enemy faction names.", details = {keys = {spec = "Factionspec"}}}
	}}

--- Creates a new faction.
-- @param clss Factionspec class.
-- @param args Arguments.
-- @return Faction spec.
Factionspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Adds an enemy faction.
-- @param self Faction spec.
-- @param args Arguments.<ul>
--   <li>name: Faction name.</li></ul>
Factionspec.add_enemy = function(self, args)
	self.enemies[args.name] = true
end

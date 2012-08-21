local Class = require("system/class")
require(Mod.path .. "spec")

Actorskinspec = Class("Actorskinspec", Spec)
Actorskinspec.type = "actorskinspec"
Actorskinspec.dict_id = {}
Actorskinspec.dict_cat = {}
Actorskinspec.dict_name = {}
Actorskinspec.introspect = Introspect{
	name = "Actorskinspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "actors", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of actor spec names.", details = {keys = {spec = "Actorspec"}}},
		{name = "material", type = "string", default = "animskin1", description = "Material name."},
		{name = "textures", type = "list", list = {type = "string"}, description = "List of texture names."}
	}}

--- Registers an actor skin.
-- @param clss Actorskinspec class.
-- @param args Arguments.
-- @return New actor skin spec.
Actorskinspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets skins by actor type.
-- @param self Actorskinspec class.
-- @return List of skins.
Actorskinspec.find_by_actor = function(self, name)
	local res = {}
	for k,v in pairs(self.dict_name) do
		if v.actors[name] then table.insert(res, v) end
	end
	table.sort(res, function(a,b) return a.name < b.name end)
	return res
end

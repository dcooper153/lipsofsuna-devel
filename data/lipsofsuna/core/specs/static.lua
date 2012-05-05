require(Mod.path .. "spec")

Staticspec = Class(Spec)
Staticspec.class_name = "Staticspec"
Staticspec.type = "static"
Staticspec.dict_id = {}
Staticspec.dict_cat = {}
Staticspec.dict_name = {}
Staticspec.introspect = Introspect{
	name = "Staticspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "collision_group", type = "number", default = 0x2000, description = "Collision group."},
		{name = "collision_mask", type = "number", default = 0xFF, description = "Collision mask."},
		{name = "dialog", type = "string", description = "Dialog name.", details = {spec = "Dialogspec"}},
		{name = "marker", type = "string", description = "Map marker name."},
		{name = "model", type = "string", description = "Model to use for the obstacle."},
		{name = "position", type = "vector", default = Vector(), description = "Position vector."},
		{name = "rotation", type = "quaternion", default = Quaternion(), description = "Rotation quaternion."},
		{name = "usages", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of ways how the object can be used.", details = {keys = {spec = "Actionspec"}}}
	}}

--- Creates a new static map object specification.
-- @param clss Staticspec class.
-- @param args Arguments.
-- @return New item specification.
Staticspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets the use actions applicable to the static.
-- @param self Static spec.
-- @return List of actions specs.
Staticspec.get_use_actions = function(self)
	local res = {}
	for k,v in pairs(self.usages) do
		local a = Actionspec:find{name = k}
		if a then table.insert(res, a) end
	end
	return res
end

--- Gets the use actions applicable to the static.
-- @param self Static spec.
-- @return List of actions specs.
Staticspec.get_use_actions = function(self)
	local res = {}
	for k,v in pairs(self.usages) do
		local a = Actionspec:find{name = k}
		if a then table.insert(res, a) end
	end
	return res
end

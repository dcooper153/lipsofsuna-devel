require(Mod.path .. "spec")

Questspec = Class(Spec)
Questspec.type = "quest"
Questspec.dict_id = {}
Questspec.dict_cat = {}
Questspec.dict_name = {}
Questspec.introspect = Introspect{
	name = "Questspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "status", type = "string", default = "unused", description = "Quest status. (unused/inactive/active/completed)"},
		{name = "text", type = "string", default = "", description = "Textual description of the quest status."}
	}}

Quest = Questspec --FIXME

--- Creates a new quest specification.
-- @param clss Quest class.
-- @param args Arguments.
-- @return New quest spec.
Questspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

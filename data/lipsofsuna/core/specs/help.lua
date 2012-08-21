local Class = require("system/class")
require(Mod.path .. "spec")

Helpspec = Class("Helpspec", Spec)
Helpspec.type = "helpspec"
Helpspec.dict_id = {}
Helpspec.dict_cat = {}
Helpspec.dict_name = {}
Helpspec.introspect = Introspect{
	name = "Helpspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "text", type = "string", description = "Help text."},
		{name = "title", type = "string", description = "Help page title."}
	}}

--- Registers a help specifification.
-- @param clss Helpspec class.
-- @param args Arguments.
-- @return New skill spec.
Helpspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

require(Mod.path .. "spec")

Actionspec = Class(Spec)
Actionspec.class_name = "Actionspec"
Actionspec.type = "action"
Actionspec.dict_id = {}
Actionspec.dict_cat = {}
Actionspec.dict_name = {}
Actionspec.introspect = Introspect{
	name = "Actionspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "dialog", type = "boolean", description = "True to make the client show the dialog of the object."},
		{name = "func", type = "ignore"},
		{name = "label", type = "string", description = "Human readable name of the action."}
	}}

--- Creates a new action specification.
-- @param clss Actionspec class.
-- @param args Arguments.<ul>
--   <li>categories: List of categories to which the action belongs.</li>
--   <li>func: Function to handle the action.</li>
--   <li>name: Name of the action type.</li></ul>
-- @return New action specification.
Actionspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

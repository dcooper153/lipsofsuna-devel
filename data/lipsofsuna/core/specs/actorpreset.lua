require(Mod.path .. "spec")

Actorpresetspec = Class(Spec)
Actorpresetspec.type = "actorpresetspec"
Actorpresetspec.dict_id = {}
Actorpresetspec.dict_cat = {}
Actorpresetspec.dict_name = {}
Actorpresetspec.introspect = Introspect{
	name = "Actorpresetspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "body", type = "list", list = {type = "number"}, description = "Body morph table."},
		{name = "eye_color", type = "color", description = "Eye color."},
		{name = "face", type = "list", list = {type = "number"}, description = "Face morph table."},
		{name = "hair_color", type = "color", description = "Hair color."},
		{name = "hair_style", type = "string", description = "Hair style name."},
		{name = "head_style", type = "string", description = "Head style name."},
		{name = "height", type = "number", description = "Height multiplier."},
		{name = "playable", type = "boolean", description = "True to show the preset in the character creation screen."},
		{name = "skin_color", type = "color", description = "Skin color."},
		{name = "skin_style", type = "string", description = "Skin style name."}
	}}

--- Registers an actor preset.
-- @param clss Actorpresetspec class.
-- @param args Arguments.
-- @return New actor preset spec.
Actorpresetspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

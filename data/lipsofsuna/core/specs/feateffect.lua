require(Mod.path .. "spec")

Feateffectspec = Class(Spec)
Feateffectspec.type = "feateffect"
Feateffectspec.dict_id = {}
Feateffectspec.dict_cat = {}
Feateffectspec.dict_name = {}
Feateffectspec.introspect = Introspect{
	name = "Feateffectspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "animations", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of compatible feat types.", details = {keys = {spec = "Feattypespec"}}},
		{name = "affects_allies", type = "boolean", description = "True if the effect is applicable to allied creatures."},
		{name = "affects_enemies", type = "boolean", description = "True if the effect is applicable to enemy creatures."},
		{name = "affects_items", type = "boolean", description = "True if the effect is applicable to items."},
		{name = "affects_terrain", type = "boolean", description = "True if the effect is applicable to terrain."},
		{name = "cooldown", type = "number", default = 0, description = "Cooldown time in seconds."},
		{name = "description", type = "string", description = "Description of the feat effect."},
		{name = "duration", type = "number", default = 0, description = "Duration of the created spell object, in seconds."},
		{name = "effect", type = "string", description = "Named of the sound effect to play."},
		{name = "icon", type = "string", description = "Icon name.", default = "missing1", details = {spec = "Iconspec"}},
		{name = "influences", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of influences and their strengths.", details = {keys = {spec = "Feateffectspec"}}},
		{name = "projectile", type = "string", description = "Spell object to use as a projectile.", details = {spec = "Spellspec"}},
		{name = "radius", type = "number", default = 0, description = "Area of effect radius."},
		{name = "range", type = "number", default = 0, description = "Maximum firing range for bullet and ray targeting modes."},
		{name = "required_reagents", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of required reagents.", details = {keys = {spec = "Itemspec"}}},
		{name = "required_stats", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of required stats."}
	}}

--- Registers a feat effect.
-- @param clss Feateffectspec class.
-- @param args Arguments.
-- @return New feat effect.
Feateffectspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	-- Store the effect to the compatible animations.
	for k,v in pairs(self.animations) do
		local a = Feattypespec:find{name = k}
		if a then a.effects[self.name] = self end
	end
	return self
end

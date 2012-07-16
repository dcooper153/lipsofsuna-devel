require "system/model"

if not Los.program_load_extension("model-merge") then
	error("loading extension `model-merge' failed")
end

------------------------------------------------------------------------------

Merger = Class()
Merger.class_name = "Merger"

--- Creates a new model merger.
-- @param clss Merger class.
-- @return New model merger.
Merger.new = function(clss)
	local self = Class.new(Merger)
	self.handle = Los.merger_new()
	return self
end

--- Adds a model.
-- @param self Model merger.
-- @param model Model to add.
Merger.add_model = function(self, model)
	Los.merger_add_model(self.handle, model.handle)
end

--- Adds a morphed model.
-- @param self Model merger.
-- @param model Model to add.
-- @param targets List of alternating morph target names and influence.
Merger.add_model_morph = function(self, model, targets)
	Los.merger_add_model_morph(self.handle, model.handle, unpack(targets))
end

--- Queues finishing the build.
-- @param self Model merger.
Merger.finish = function(self)
	Los.merger_finish(self.handle)
end

--- Pops a finished model.
-- @param self Model merger.
-- @return Model or nil.
Merger.pop_model = function(self, model, targets)
	local handle = Los.merger_pop_model(self.handle)
	if not handle then return end
	return Class.new(Model, {handle = handle})
end

--- Replaces fields of matching materials.
-- @param self Model merger.
-- @param args Arguments.<ul>
--   <li>diffuse: Diffuse color to set, or nil.</li>
--   <li>match_material: Material reference name to match.</li>
--   <li>material: Material reference name to set, or nil.</li>
--   <li>specular: Specular color to set, or nil.</li>
--   <li>textures: Array of texture to set, or nil.</li></ul>
Merger.replace_material = function(self, args)
	Los.merger_replace_material(self.handle, args)
end

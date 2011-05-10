require "system/model"
require "system/object"

if not Los.program_load_extension("object-render") then
	error("loading extension `object-render' failed")
end

------------------------------------------------------------------------------

--- Edits the matching material.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>diffuse: Diffuse color to set or nil.</li>
--   <li>match_shader: Shader name to match or nil.</li>
--   <li>match_texture: Texture name to match or nil.</li>
--   <li>specular: Specular color to set or nil.</li>
Model.edit_material = function(self, args)
	Los.model_edit_material(self.handle, args)
end

--- Morphs a model with one of its shape keys.
-- @param self Object.
-- @param ... Arguments.<ul>
--   <li>1,shape: Shape key name.</li>
--   <li>2,value: Shape influence multiplier.</li>
--   <li>3,ref: Reference model for relative morphing, or nil.</ul>
Model.morph = function(self, ...)
	local a,b,c = ...
	if type(a) == "table" then
		Los.model_morph(self.handle, a.shape, a.value, a.ref and a.ref.handle)
	else
		Los.model_morph(self.handle, a, b, c and c.handle)
	end
end

------------------------------------------------------------------------------

--- Deforms the mesh of the object according to its animation pose.
-- @param self Object.
Object.deform_mesh = function(self)
	return Los.object_deform_mesh(self.handle)
end

--- Starts the particle animation of the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>loop: True to loop infinitely.</li>
--   <li>time: Animation offset in seconds.</li></ul>
Object.particle_animation = function(self, args)
	return Los.object_particle_animation(self.handle, args)
end

--- Recalculates the positions of transparent faces for correct depth sorting.
-- @param self Object.
Object.update_transparency = function(self)
	return Los.object_update_transparency(self.handle)
end

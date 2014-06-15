--- Renderable object.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.render_object
-- @alias RenderObject

local Animation = require("system/animation")
local Class = require("system/class")
local Quaternion = require("system/math/quaternion")

if not Los.program_load_extension("graphics") then
	error("loading extension `graphics' failed")
end

if not Los.program_load_extension("render-object") then
	error("loading extension `render-object' failed")
end

------------------------------------------------------------------------------

--- Renderable object.
-- @type RenderObject
local RenderObject = Class("RenderObject")

--- Creates a new render object.
-- @param self Render object class.
-- @return Render object.
RenderObject.new = function(clss)
	local self = Class.new(clss)
	self.handle = Los.render_object_new()
	return self
end

--- Adds a model to the object.
-- @param self Object.
-- @param model Model.
RenderObject.add_model = function(self, model)
	if not model then return end
	Los.render_object_add_model(self.handle, model.handle)
end

--- Adds a permanent texture alias.
--
-- The alias causes the graphics engine to invoke replace_texture() with the
-- given arguments for all current and future models attached to the object.
-- Contrast to replace_texture(), which only affects the current models.
--
-- @param self Render object.
-- @param name Texture name without extension.
-- @param image Image.
RenderObject.add_texture_alias = function(self, name, image)
	Los.render_object_add_texture_alias(self.handle, name, image.handle)
end

--- Sets or clears an animation.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>animation: Animation name.</li>
--   <li>blend_mode: Blending mode.</li>
--   <li>channel: Channel number.</li>
--   <li>fade_in: Fade in duration in seconds.</li>
--   <li>fade_in_mode: Fade in mode as a string.</li>
--   <li>fade_out: Fade out duration in seconds.</li>
--   <li>fade_out_mode: Fade in mode as a string.</li>
--   <li>node_priorities: Sets blending priorities for individual nodes.</li>
--   <li>node_weight: Sets blending weight multipliers for individual nodes.</li>
--   <li>permanent: True if should keep repeating.</li>
--   <li>priority: Blending priority of rotation and translation.</li>
--   <li>priority_scale: Blending priority of scaling.</li></ul>
--   <li>repeat_start: Starting time when repeating.</li>
--   <li>replace: Completely replace the overwritten animation.</li>
--   <li>time: Starting time.</li>
--   <li>time_scale: Time scaling factor.</li>
--   <li>weight: Blending weight of rotation and translation.</li>
--   <li>weight_scale: Blending weight of scaling.</li></ul>
-- @return True if started a new animation.
RenderObject.animate = function(self, args)
	if type(args.animation) == "string" then
		local anim = Animation.dict_name[args.animation] or Animation:load(args.animation)
		args.animation = anim and anim.handle or nil
	elseif args.animation then
		args.animation = args.animation.handle
	end
	return Los.render_object_animate(self.handle, args)
end

--- Fades out an animation channel.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>channel: Channel number.</li>
--   <li>duration: Fade duration in seconds.</li></ul>
RenderObject.animate_fade = function(self, args)
	return Los.render_object_animate_fade(self.handle, args)
end

--- Removes all animation channels and fades.
-- @param self Object.
RenderObject.clear_animations = function(self)
	return Los.render_object_clear_animations(self.handle)
end

--- Finds a bone or an anchor by name.
-- @param self Object.
-- @param name Node name.
-- @return Position and rotation, or nil if not found.
RenderObject.find_node_by_name = function(self, name)
	local p,r = Los.render_object_find_node(self.handle, name)
	if not p then return end
	return Vector:new_from_handle(p), Quaternion:new_from_handle(r)
end

--- Finds a bone or an anchor by name.
-- @param self Object.
-- @param name Node name.
-- @return Position and rotation, or nil if not found.
RenderObject.find_node_by_name_world_space = function(self, name)
	local p,r = Los.render_object_find_node_world_space(self.handle, name)
	if not p then return end
	return Vector:new_from_handle(p), Quaternion:new_from_handle(r)
end

--- Gets animation information for the given animation channel.<br/>
-- If an animation is looping in the channel, a table containing the fields
-- animation, time, and weight is returned.
-- @param self Server class.
-- @param args Arguments.<ul>
--   <li>channel: Channel number. (required)</li></ul>
-- @return Animation info table or nil.
RenderObject.get_animation = function(self, args)
	return Los.render_object_get_animation(self.handle, args)
end

--- Starts the particle animation of the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>loop: True to loop infinitely.</li>
--   <li>time: Animation offset in seconds.</li></ul>
RenderObject.particle_animation = function(self, args)
	return Los.render_object_particle_animation(self.handle, args)
end

--- Removes a model from the object.
-- @param self Object.
-- @param model Model.
RenderObject.remove_model = function(self, model)
	if not model then return end
	Los.render_object_remove_model(self.handle, model.handle)
end

--- Removes a model from the object.
-- @param self Object.
-- @param model_old Model.
-- @param model_new Model.
RenderObject.replace_model = function(self, model_old, model_new)
	Los.render_object_replace_model(self.handle,
		model_old and model_old.handle or nil,
		model_new and model_new.handle or nil)
end

--- Replaces a texture with an image.
-- @param self Render object.
-- @param name Texture name without extension.
-- @param image Image.
RenderObject.replace_texture = function(self, name, image)
	Los.render_object_replace_texture(self.handle, name, image.handle)
end

--- Sets a custom shader parameter.
-- @param self Object.
-- @param index Parameter index.
-- @param r Parameter value.
-- @param g Parameter value.
-- @param b Parameter value.
-- @param a Parameter value.
RenderObject.set_custom_param = function(self, index, r, g, b, a)
	Los.render_object_set_custom_param(self.handle, index, r, g, b, a)
end

--- Set or clears the additional effect layer of the object.
-- @param self Object.
-- @param ... Arguments.<ul>
--   <li>params: Array numbers to be passed to the shader.</li>
--   <li>shader: Shader name.</li></ul>
RenderObject.set_effect = function(self, ...)
	Los.render_object_set_effect(self.handle, ...)
end

--- Gets the particle effect name of the object.
-- @param self Object.
-- @return Particle effect name.
RenderObject.get_particle = function(self)
	return rawget(self, "__particle")
end

--- Gets the particle emitting state for the object.
-- @param self Object.
-- @return True if enabled, false if disable.
RenderObject.get_particle_emitting = function(self)
	return rawget(self, "__particle_emitting")
end

--- Gets the position of the object.
-- @param self Object.
-- @return Vector.
RenderObject.get_position = function(self)
	local v = rawget(self, "__position")
	if not v then
		v = Vector()
		rawset(self, "__position", v)
	end
	return v
end

--- Gets the maxium render distance of the object.
-- @param self Object.
-- @return Render distance in world units.
RenderObject.get_render_distance = function(self)
	return rawget(self, "__render_distance")
end

--- Returns true if the render data of the object has been loaded.
-- @param self Object.
-- @return True if loaded, false if not.
RenderObject.get_loaded = function(self)
	return Los.render_object_get_render_loaded(self.handle)
end

--- Gets the rotation of the object.
-- @param self Object.
-- @return Quaternion.
RenderObject.get_rotation = function(self)
	local v = rawget(self, "__rotation")
	if not v then
		v = Quaternion()
		rawset(self, "__rotation", v)
	end
	return v
end

--- Gets the shadow casting mode of the object.
-- @param self Object.
-- @return True if enabled, false if disable.
RenderObject.get_shadow_casting = function(self)
	local v = rawget(self, "__shadow_casting")
	return (v ~= nil) and v or false
end

--- Gets the visibility status of the object.
-- @param self Object.
-- @return True if visible, false if not.
RenderObject.get_visible = function(self)
	return rawget(self, "__visible")
end

--- Sets the particle effect of the object.
-- @param self Object.
-- @param v Particle effect name.
RenderObject.set_particle = function(self, v)
	rawset(self, "__particle", v)
	rawset(self, "__particle_emitting", true)
	rawset(self, "__model", nil)
	Los.render_object_set_particle(self.handle, v)
end

--- Toggles particle emitting for the object.
-- @param self Object.
-- @param v True to enable, false to disable.
RenderObject.set_particle_emitting = function(self, v)
	rawset(self, "__particle_emitting", v)
	Los.render_object_set_particle_emitting(self.handle, v)
end

--- Sets the position of the object.
-- @param self Object.
-- @param v Vector.
RenderObject.set_position = function(self, v)
	rawset(self, "__position", v)
	Los.render_object_set_position(self.handle, v.handle)
end

--- Sets the rotation of the object.
-- @param self Object.
-- @param v Quaternion.
RenderObject.set_rotation = function(self, v)
	rawset(self, "__rotation", v)
	Los.render_object_set_rotation(self.handle, v.handle)
end

--- Specifies the maximum render distance of the object.
-- @param self Object.
-- @param v Render distance in world units.
RenderObject.set_render_distance = function(self, v)
	rawset(self, "__render_distance", v)
	Los.render_object_set_render_distance(self.handle, v)
end

--- Gets the render queue of the object.
-- @param self Object.
-- @return Queue name.
RenderObject.get_render_queue = function(self)
	return self.__render_queue or "main"
end

--- Sets the render queue of the object.
-- @param self Object.
-- @param value Queue name.
RenderObject.set_render_queue = function(self, value)
	self.__render_queue = value
	Los.render_object_set_render_queue(self.handle, value)
end

--- Enables or disables shadow casting for the object.
-- @param self Object.
-- @param v True to enable, false to disable.
RenderObject.set_shadow_casting = function(self, v)
	rawset(self, "__shadow_casting", v)
	Los.render_object_set_shadow_casting(self.handle, v)
end

--- Sets the visibility status of the object.
-- @param self Object.
-- @param v True if visible, false if not.
RenderObject.set_visible = function(self, v)
	rawset(self, "__visible", v)
	Los.render_object_set_visible(self.handle, v)
end

return RenderObject



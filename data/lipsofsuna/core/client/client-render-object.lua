--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.client_render_object
-- @alias ClientRenderObject

local Animation = require("system/animation")
local Class = require("system/class")
local Model = require("system/model")
local ModelBuilder = require("character/model-builder") --FIXME
local ModelEffect = require("core/effect/model-effect") --FIXME
local ParticleEffect = require("core/effect/particle-effect") --FIXME
local RenderObject = require("system/render-object")
local RenderUtils = require("core/client/render-utils")
local SpeedlineEffect = require("core/effect/speedline-effect") --FIXME
local TextureBuilder = require("character/texture-builder") --FIXME

--- TODO:doc
-- @type ClientRenderObject
local ClientRenderObject = Class("ClientRenderObject", RenderObject)

--- Initializes the render object.
-- @param self Render object.
-- @param object Object.
ClientRenderObject.init = function(self, object)
	-- Mark as initialized.
	if self.initialized then self:clear() end
	if not object.spec then return end
	self.initialized = true
	-- Store the object.
	self.object = object
	Client.options:apply_object(object)
	-- Set the render model.
	if object.spec.models then
		self:request_model_rebuild()
	elseif object:get_model() then
		self:set_model(object:get_model():get_render())
	else
		self:set_model()
	end
	self:set_visible(true)
	-- Create special effects.
	self.special_effects = RenderUtils:create_special_effects(self, object.spec)
	-- Create equipment anchors.
	for slot,index in pairs(self.object.inventory.equipped) do
		local item = self.object.inventory:get_object_by_index(index)
		if item then
			self:handle_inventory_equip(slot, item)
		end
	end
	-- Create spell particles.
	if object.spec and object.spec.particle then
		self:add_effect(ParticleEffect{
			parent = self,
			particle = object.spec.particle,
			position_mode = "node"})
	end
end

--- Clears the render object.
-- @param self Render object.
ClientRenderObject.clear = function(self)
	-- Mark as uninitialized.
	if not self.initialized then return end
	self.initialized = nil
	self.object.model_build_hash = nil
	self.object.texture_build_hash = nil
	self.object = nil
	-- Reset and hide the render object.
	self:clear_animations()
	self:set_visible(false)
	-- Remove the model.
	if self.model then
		self:remove_model(self.model)
		self.model = nil
	end
	-- Detach special effects.
	if self.special_effects then
		for k,v in pairs(self.special_effects) do v:unparent() end
		self.special_effects = nil
	end
	-- Stop the speed line
	if self.speedline then
		self.speedline.object = nil
		self.speedline = nil
	end
end

--- Plays an animation by name from the spec of the object.
-- @param self Render object.
-- @param name Animation name.
-- @param time Optional start time, or nil.
-- @param variant Animation variant number, or nil.
-- @param weapon Weapon being wielded, or nil.
-- @return Animation arguments.
ClientRenderObject.add_animation = function(self, name, time, variant, weapon)
	-- Check for initialization.
	if not self.initialized then return end
	if not self.object.spec.get_animation_arguments then return end
	-- Determine the animation profile.
	--
	-- Animation selection is based on the animation profile name provided by
	-- the actor. The name determines from which profile the animations will
	-- be taken. Actors can easily customize their animations by changing the
	-- animation profile they are using.
	local profile = self.object:get_animation_profile()
	-- Get the animation arguments.
	--
	-- Animations may be overridden by the wielded weapon. In such a case, the
	-- spec of the weapon will return an equipment animation that will be used.
	-- Otherwise, the animation is obtained from the actor spec.
	local args
	if weapon then
		args = weapon.spec:get_animation_arguments_equipped(name, profile, variant)
	end
	if not args then
		args = self.object.spec:get_animation_arguments(name, profile, variant)
	end
	-- Set the time offset.
	if time and time > 0 then
		args.time = (args.time or 0) + time
		args.fade_in = 0
	end
	-- Animate the render model.
	self:animate(args)
	return args
end

--- Adds an effect anchor to the object.
-- @param self Render object.
-- @param effect Effect anchor.
ClientRenderObject.add_effect = function(self, effect)
	if not self.initialized then return end
	table.insert(self.special_effects, effect)
end

--- Adds a wielded weapon to the model.
-- @param self Render object.
-- @param object Wielded item.
-- @param slot Equipment slot name.
-- @param node Node where to attach the object.
ClientRenderObject.add_equipment_anchor = function(self, object, slot, node)
	if not self.initialized then return end
	-- Get the anchored model.
	local model_name = object:get_model_name()
	if not model_name then return end
	local model = Main.models:find_by_name(model_name)
	if not model then return end
	-- Add the anchor effect.
	local effect = ModelEffect{
		model = model,
		model_node = object.spec.equipment_anchor,
		parent = self,
		parent_node = node,
		position_local = object.spec.equipment_anchor_position,
		position_mode = "node-node",
		rotation_local = object.spec.equipment_anchor_rotation,
		rotation_mode = "node-node"}
	effect.anchor_object = object
	if self.special_effects then
		table.insert(self.special_effects, effect)
	else
		self.special_effects = {effect}
	end
	-- Add special effects for the anchor.
	effect:add_special_effects(RenderUtils:create_special_effects(effect, object.spec))
	-- Add the equipment holding animation.
	local anim = self:add_animation("hold", 0, 0, object)
	if anim then
		if self.equipment_animations then
			self.equipment_animations[slot] = anim
		else
			self.equipment_animations = {[slot] = anim}
		end
	else
		if not self.equipment_animations then return end
		if not self.equipment_animations[slot] then return end
		self:animate_fade{channel = self.equipment_animations[slot].channel}
		self.equipment_animations[slot] = nil
	end
end

--- Adds a speed line effect for the object.
-- @param self Render object.
-- @param args Speed line construction arguments.
ClientRenderObject.add_speedline = function(self, args)
	if not self.initialized then return end
	-- Stop the old speedline
	if self.speedline then
		self.speedline.object = nil
		self.speedline = nil
	end
	-- Create the new speedline
	if not args then args = {} end
	args.object = self.object
	self.speedline = SpeedlineEffect(args)
end

--- Gets the bounding box of the object.
-- @param self Render object.
-- @return Bounding box, or nil.
ClientRenderObject.get_bounding_box = function(self)
	if not self.initialized then return end
	if not self.model then return end
	return self.model.bounding_box
end

--- Gets the anchor effect for the given object.
-- @param self Render object.
-- @param object Wielded item.
-- @return Effect object, or nil.
ClientRenderObject.get_equipment_anchor = function(self, object)
	if not self.initialized then return end
	for k,v in pairs(self.special_effects) do
		if v.anchor_object == object then
			return v
		end
	end
end

--- Gets the render model of the object.
-- @param self Render object.
-- @return Render model, or nil.
ClientRenderObject.get_model = function(self)
	return self.model
end

ClientRenderObject.handle_inventory_equip = function(self, slot, object)
	if not self.initialized then return end
	local node = self.object.spec:get_node_by_equipment_slot(slot)
	if node then
		self:add_equipment_anchor(object, slot, node)
	else
		self:request_model_rebuild()
	end
end

ClientRenderObject.handle_inventory_unequip = function(self, slot, object)
	if not self.initialized then return end
	local node = self.object.spec:get_node_by_equipment_slot(slot)
	if node then
		self:remove_equipment_anchor(object, slot, node)
	else
		self:request_model_rebuild()
	end
end

--- Removes a wielded weapon from the model.
-- @param self Render object.
-- @param object Wielded item.
-- @param slot Equipment slot name.
-- @param node Node where to attach the object.
ClientRenderObject.remove_equipment_anchor = function(self, object, slot, node)
	if not self.initialized then return end
	-- Remove the anchor effect.
	for k,v in pairs(self.special_effects) do
		if v.anchor_object == object then
			table.remove(self.special_effects, k)
			v:detach()
			break
		end
	end
	-- Disable the equipment holding animation.
	if not self.equipment_animations then return end
	if not self.equipment_animations[slot] then return end
	self:animate_fade{channel = self.equipment_animations[slot].channel}
	self.equipment_animations[slot] = nil
end

--- Queues a model rebuild for the actor.
-- @param self Render object.
ClientRenderObject.request_model_rebuild = function(self)
	if not self.initialized then return end
	if not self.object.spec.models then return end
	self.model_rebuild_timer = 0.1
end

--- Updates the state of the render object.
-- @param self Render object.
-- @param secs Seconds since the last update.
ClientRenderObject.update = function(self, secs)
	if not self.initialized then return end
	-- Handle model and texture rebuilding.
	--
	-- Equipment changes can occur frequently when newly appearing actors are
	-- being setup. Because of that, a delay is used to avoid too many expensive
	-- rebuilds.
	if self.model_rebuild_timer then
		self.model_rebuild_timer = self.model_rebuild_timer - secs
		if self.model_rebuild_timer <= 0 then
			self.model_rebuild_timer = nil
			if self.object.spec.models then
				-- Build the character model in a separate thread.
				-- The result is handled in the tick handler below.
				ModelBuilder:build_for_actor(self.object)
				-- Build the character texture in a separate thread.
				TextureBuilder:build_for_actor(self.object)
			end
		end
	end
	-- Apply built models.
	--
	-- Models are built asynchronously to avoid stuttering when multiple models
	-- are built at the same time. As building is completed, the merged model
	-- can be popped from the model merger of the object.
	if self.object.model_merger then
		local m = self.object.model_merger:pop_model()
		if m then
			self:set_model(m:get_render())
			self.model.bounding_box = m:get_bounding_box()
		end
	end
	-- Apply built textures.
	--
	-- Textures are built asynchronously similar to models. The build result
	-- is handled here.
	if self.object.image_merger then
		local image = self.object.image_merger:pop_image()
		if image then
			local spec = self.object:get_spec()
			local base = spec:get_base_texture()
			if base then
				self:add_texture_alias(base, image)
			end
		end
	end
end

--- Updates the body and head scale of the object.
-- @param self Render object.
ClientRenderObject.update_scale = function(self)
	if not self.initialized then return end
	local args = RenderUtils:create_scale_animation(self.object.spec, self.object.body_scale, self.object.head_scale)
	if args then
		self:animate(args)
	end
	local args = RenderUtils:create_breast_animation(self.object)
	if args then
		self:animate(args)
	end
end

--- Sets the beheading state of the object.
-- @param self Render object.
-- @param value Boolean.
ClientRenderObject.set_beheaded = function(self, value)
	if self.beheaded == value then return end
	self.beheaded = value
	self:request_model_rebuild()
end

--- Sets the render model of the object.
-- @param self Render object.
-- @param value Render model.
ClientRenderObject.set_model = function(self, value)
	if not self.initialized then return end
	-- Set the render model.
	local prev_model = self.model
	if self.model ~= value then
		if self.model and value then
			self:replace_model(self.model, value)
		elseif value then
			self:add_model(value)
		elseif self.model then
			self:remove_model(self.model)
		end
		self.model = value
	end
	-- Set the scale animation.
	self:update_scale()
end

--- Sets the tilt angle of the object.
-- @param self Render object.
-- @param nodes Back nodes.
-- @param value Tilt angle in radians.
ClientRenderObject.set_tilt = function(self, nodes, value)
	if not self.initialized then return end
	if not nodes then return end
	if not nodes[1] then return end
	-- Calculate the bone rotation.
	local rot = Quaternion{axis = Vector(1,0,0), angle = -value / #nodes}
	-- Create the tilting channel.
	local anim = Animation("tilt")
	for k,v in pairs(nodes) do
		anim:set_transform{frame = 1, node = v, rotation = rot}
	end
	self:animate{animation = anim, channel = 127, blend_mode = "add",
		fade_in = 0, fade_out = 0, permanent = true, priority = 1000, replace = true, weight = 1}
end

return ClientRenderObject

--- Visuals of a specific object.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.object_visual
-- @alias ObjectVisual

local Animation = require("system/animation")
local CensorshipEffect = require("core/effect/censorship-effect")
local Class = require("system/class")
local Color = require("system/color")
local HairStyleSpec = require("core/specs/hair-style")
local Model = require("system/model")
local ModelBuilder = require("character/model-builder")
local ModelEffect = require("core/effect/model-effect")
local ParticleEffect = require("core/effect/particle-effect")
local RenderObject = require("system/render-object")
local RenderUtils = require("core/scene/render-utils")
local SoftbodyEffect = require("core/effect/softbody-effect")
local SpeedlineEffect = require("core/effect/speedline-effect")

--- Visuals of a specific object.
-- @type ObjectVisual
local ObjectVisual = Class("ObjectVisual")

--- Creates new object visuals.
-- @param clss ObjectVisual class.
-- @return ObjectVisual.
ObjectVisual.new = function(clss, object, render)
	local self = Class.new(clss)
	self.object = object
	self.render = render
	self.slots = {}
	self.__softbody_enabled = Main.client.options.softbody_enabled
	self.__hair_color = {255, 255, 255}
	return self
end

--- Adds an anchor object.
-- @param self ObjectVisual.
-- @param slot Slot name.
-- @param object Object.
-- @param node Node name.
ObjectVisual.add_anchor_object = function(self, slot, object, node)
	-- Get the anchored model.
	local model_name = object:get_model_name()
	if not model_name then return end
	local model = Main.models:find_by_name(model_name)
	if not model then return end
	-- Create the model effect.
	local effect = ModelEffect{
		model = model,
		model_node = object.spec.equipment_anchor,
		parent = self.render,
		parent_node = node,
		position_local = object.spec.equipment_anchor_position,
		position_mode = "node-node",
		rotation_local = object.spec.equipment_anchor_rotation,
		rotation_mode = "node-node"}
	effect.anchor_object = object
	self:__set_slot(slot, effect)
	-- Add special effects for the anchor.
	effect:add_special_effects(RenderUtils:create_special_effects(effect, object.spec))
end

--- Adds a censorship effect.
-- @param self ObjectVisual.
-- @param slot Slot name.
-- @param node Node name.
ObjectVisual.add_censorship = function(self, slot, node)
	self:__set_slot(slot, CensorshipEffect(self.render, node))
end

--- Adds a hair style.
-- @param self ObjectVisual.
-- @param slot Slot name.
-- @param style Hair style name.
ObjectVisual.add_hair_style = function(self, style)
	-- Get the hair style spec.
	if not style then return end
	local spec = HairStyleSpec:find_by_name(style)
	if not spec then return end
	-- Get the list of anchors.
	local class = self.object.spec.equipment_class or self.object.spec.name
	local list = spec:get_equipment_anchors(class)
	if not list then return end
	-- Create the new hair anchors.
	for k,v in pairs(list) do
		if v.partition == "head" and self.render.beheaded then
			-- Skip dismembered effects.
		elseif v.type == "model" or v.type == "softbody" then
			-- Models and softbodies.
			local copied = false
			local model = Main.models:find_by_name(v.model)
			if v.model_color == "hair" then
				if self.object.hair_color then
					local rgb = Color:hsv_to_rgb(Color:ubyte_to_float(self.object.hair_color))
					copied = true
					model = model:copy()
					model:edit_material{match_material = "animhair1", diffuse = rgb}
				end
			end
			if v.model_scale == "head" then
				local ospec = self.object.spec
				local scale = self.object.head_scale or 0.5
				if ospec.head_scale_min and ospec.head_scale_max then
					scale = ospec.head_scale_min + scale * (ospec.head_scale_max - ospec.head_scale_min)
					if scale ~= 1 then
						if not copied then
							model = model:copy()
							copied = true
						end
						model:scale(scale)
						model:calculate_bounds()
					end
				end
			end
			local effect
			if v.type == "softbody" and Main.client.options.softbody_enabled then
				local params = v.softbody_params or {}
				effect = SoftbodyEffect(self.render, v.parent_node, model, params)
			else
				effect = ModelEffect{
					model = model,
					model_node = v.model_node,
					parent = self.render,
					parent_node = v.parent_node,
					position_local = v.equipment_anchor_position,
					position_mode = "node-node",
					rotation_local = v.equipment_anchor_rotation,
					rotation_mode = "node-node"}
			end
			effect.anchor_object = self.object
			effect.partition = v.partition
			effect.weird_rotation = nil
			self:__set_slot("hair_" .. v.slot, effect)
		end
	end
end

--- Adds a particle item.
-- @param self ObjectVisual.
-- @param slot Slot name.
-- @param name Particle effect name.
ObjectVisual.add_particle_effect = function(self, slot, name)
	self:__set_slot(slot, ParticleEffect{
		parent = self.render,
		particle = name,
		position_mode = "node"})
end

--- Adds an anchor item.
-- @param self ObjectVisual.
-- @param slot Slot name.
-- @param spec HairStyleSpec.
ObjectVisual.add_softbody_hair = function(self, slot, spec)
end

--- Adds special effects.
-- @param self ObjectVisual.
-- @param slot Slot name.
-- @param spec ObjectSpec.
ObjectVisual.add_special_effects = function(self, slot, spec)
	local effects = RenderUtils:create_special_effects(self.render, spec)
	for k,v in pairs(effects) do
		self.slots[slot .. "_effect_" .. tostring(k)] = v
	end
end

--- Finds an effect by its parent object.
-- @param object Object.
-- @return Effect if found. Nil otherwise.
ObjectVisual.find_by_object = function(self, object)
	for k,v in pairs(self.slots) do
		if v.anchor_object == object then
			return v
		end
	end
end

--- Finds an effect by slot name.
-- @param slot Slot name.
-- @return Effect if found. Nil otherwise.
ObjectVisual.find_by_slot = function(self, slot)
	return self.slots[slot]
end

--- Unparents all effects.
-- @param self ObjectVisual.
ObjectVisual.unparent_all = function(self)
	for k,v in pairs(self.slots) do
		self:__unparent(v)
	end
	self.slots = {}
end

--- Unparents all effects of the given type.
-- @param self ObjectVisual.
-- @param class Class.
ObjectVisual.unparent_by_class = function(self, class)
	for k,effect in pairs(self.slots) do
		if effect.class == class then
			if effect.class == class then
				effect:detach_delayed() --FIXME
				self.slots[k] = nil
			end
		end
	end
end

--- Unparents all effects in the given partition.
-- @param self ObjectVisual.
-- @param partition Partition name.
ObjectVisual.unparent_by_partition = function(self, partition)
	for k,effect in pairs(self.slots) do
		if effect.partition == partition then
			effect:unparent()
			self.slots[k] = nil
		end
	end
end

--- Unparents an individual slot.
-- @param self ObjectVisual.
-- @param slot Slot name.
ObjectVisual.unparent_by_slot = function(self, slot)
	local effect = self.slots[slot]
	if effect then
		self.slots[slot] = nil
		self:__unparent(effect)
	end
end

--- Unparents all slots that start with the given prefix.
-- @param self ObjectVisual.
-- @param prefix Slot prefix.
ObjectVisual.unparent_by_slot_prefix = function(self, prefix)
	for slot,effect in pairs(self.slots) do
		if string.sub(slot, 1, #prefix) == prefix then
			self.slots[slot] = nil
			self:__unparent(effect)
		end
	end
end

--- Updates the anchors.
-- @param self ObjectVisual.
-- @param secs Seconds since the last update.
ObjectVisual.update = function(self, secs)
	-- Rebuild hair.
	local a = self.__hair_color
	local b1 = self.object.hair_color and self.object.hair_color[1] or 255
	local b2 = self.object.hair_color and self.object.hair_color[2] or 255
	local b3 = self.object.hair_color and self.object.hair_color[3] or 255
	if self.__beheaded ~= self.render.beheaded or
	   self.__hair_style ~= self.object.hair_style or
	   a[1] ~= b1 or a[2] ~= b2 or a[3] ~= b3 or
	   self.__head_scale ~= self.object.head_scale or
	   self.__softbody_enabled ~= Main.client.options.softbody_enabled then
		self.__beheaded = self.render.beheaded
		self.__hair_color = {b1, b2, b3}
		self.__hair_style = self.object.hair_style
		self.__head_scale = self.object.head_scale
		self.__softbody_enabled = Main.client.options.softbody_enabled
		self:unparent_by_slot_prefix("hair_")
		self:add_hair_style(self.object.hair_style)
	end
end

--- Unparents the given effect.
-- @param self ObjectVisual.
-- @param effect Effect.
ObjectVisual.__unparent = function(self, effect)
	if effect.unparent then
		effect:unparent()
	else
		effect:detach()
	end
end

--- Sets the effect of the given slot.
-- @param self ObjectVisual.
-- @param slot Slot name.
-- @param effect Effect.
ObjectVisual.__set_slot = function(self, slot, effect)
	local old = self.slots[slot]
	if old then
		self:__unparent(old)
	end
	self.slots[slot] = effect
end

return ObjectVisual

--- Animation specification.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.animation
-- @alias AnimationSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

local bone_chain_mapping = {
	LOWER = {"IK", "pelvis", "pelvis.L", "pelvis.R", "leg1.L", "leg2.L", "leg1.R", "leg2.R", "leg1", "leg2", "leg3", "leg4", "leg5", "leg6",
		"thigh.L", "thigh.R", "shin.L", "shin.R", "foot.L", "foot.R", "hips", "hips_main"},
	LEGS = {"pelvis.L", "pelvis.R", "leg1.L", "leg2.L", "leg1.R", "leg2.R", "leg1", "leg2", "leg3", "leg4", "leg5", "leg6",
		"thigh.L", "thigh.R", "shin.L", "shin.R", "foot.L", "foot.R", "hips", "hips_main"},
	ARMS = {"arm1.L", "arm2.L", "palm.L", "palm1.L", "arm1.R", "arm2.R", "palm.R", "palm1.R",
		"shoulder.L", "upper_arm.L", "forearm.L", "shoulder.R", "upper_arm.R", "forearm.R", "hand.L", "hand.R"},
	ARML = {"arm1.L", "arm2.L", "palm.L", "palm1.L",
		"shoulder.L", "upper_arm.L", "forearm.L", "hand.L"},
	ARMR = {"arm1.R", "arm2.R", "palm.R", "palm1.R",
		"shoulder.R", "upper_arm.R", "forearm.R", "hand.R"},
	BACK = {"back1", "back2", "back3",
		"spine", "chest"}}

--- Animation specification.
-- @type AnimationSpec
local AnimationSpec = Spec:register("AnimationSpec", "animation", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "animation", type = "string", description = "Animation file name."},
	{name = "animations", type = "list", list = {type = "string"}, description = "List of animation file names."},
	{name = "blend_mode", type = "string", description = "Blending mode."},
	{name = "channel", type = "number", description = "Channel number.", details = {integer = true, min = 0, max = 255}},
	{name = "fade_in", type = "number", description = "Fade in duration."},
	{name = "fade_in_frames", type = "number", description = "Fade in duration in frames."},
	{name = "fade_in_mode", type = "string", description = "Fade in mode."},
	{name = "fade_out", type = "number", description = "Fade out duration."},
	{name = "fade_out_mode", type = "string", description = "Fade out mode."},
	{name = "fade_out_frames", type = "number", description = "Fade out duration in frames."},
	{name = "node_priorities", type = "dict", dict = {type = "number"}, description = "Blending priorities for individual bones."},
	{name = "node_weights", type = "dict", dict = {type = "number"}, description = "Blending weights for individual bones."},
	{name = "permanent", type = "boolean", description = "Enables looping of the animation."},
	{name = "priority", type = "number", description = "Blending priority."},
	{name = "repeat_end", type = "number", description = "Repeat range end offset."},
	{name = "repeat_end_frames", type = "number", description = "Repeat range end offset in frames."},
	{name = "repeat_start", type = "number", description = "Repeat range start offset."},
	{name = "repeat_start_frames", type = "number", description = "Repeat range start offset in frames."},
	{name = "replace", type = "boolean", description = "True to replace the old animation, false to inherit it when possible."},
	{name = "time", type = "number", description = "Starting time offset."},
	{name = "time_frames", type = "number", description = "Starting time offset in frames."},
	{name = "time_scale", type = "number", description = "Time scale multiplier."},
	{name = "weight", type = "number", description = "Blending weight."},
	{name = "weight_scale", type = "number", description = "Blending weight for scaling."}
})

--- Creates a new animation spec.
-- @param clss AnimationSpec class.
-- @param args Arguments.
-- @return Animation spec.
AnimationSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	local s = 0.02
	if self.fade_in_frames then self.fade_in = self.fade_in_frames * s end
	if self.fade_out_frames then self.fade_out = self.fade_out_frames * s end
	if self.repeat_end_frames then self.repeat_end = self.repeat_end_frames * s end
	if self.repeat_start_frames then self.repeat_start = self.repeat_start_frames * s end
	if self.time_frames then self.time = self.time_frames * s end
	return self
end

--- Gets the animation playback arguments of the animation.
-- @param self AnimationSpec.
-- @param variant Variant number, or nil.
-- @return Table of playback arguments.
AnimationSpec.get_arguments = function(self, variant)
	local res = {
		animation = self.animation,
		blend_mode = self.blend_mode,
		channel = self.channel,
		fade_in = self.fade_in,
		fade_in_mode = self.fade_in_mode,
		fade_out = self.fade_out,
		fade_out_mode = self.fade_out_mode,
		node_priorities = self:get_node_priorities(),
		node_weights = self:get_node_weights(),
		permanent = self.permanent,
		priority = self.priority,
		repeat_end = self.repeat_end,
		repeat_start = self.repeat_start,
		replace = self.replace,
		time = self.time,
		time_scale = self.time_scale,
		weight = self.weight,
		weight_scale = self.weight_scale}
	if variant and self.animations then
		res.animation = self.animations[variant % #self.animations + 1]
	end
	return res
end

--- Gets the node priorities of the animation.
--
-- The node priority dictionary may contain some helper weights that set multiple
-- real priorities at once. If one of the recognized uppercase node names is seen
-- in the node priority list, it's replaced by this function. The returned table
-- contains the real node priorities.
--
-- @param self AnimationSpec.
-- @return Node priorities, or nil.
AnimationSpec.get_node_priorities = function(self)
	if not self.node_priorities then return end
	local w = {}
	for k,v in pairs(self.node_priorities) do
		local replace = bone_chain_mapping[k]
		if replace then
			for k1,v1 in ipairs(replace) do w[v1] = v end
		else
			w[k] = v
		end
	end
	for k in pairs(bone_chain_mapping) do
		w[k] = nil
	end
	return w
end

--- Gets the node weights of the animation.
--
-- The node weight dictionary may contain some helper weights that set multiple
-- real weights at once. If one of the recognized uppercase node names is seen
-- in the node weight list, it's replaced by this function. The returned table
-- contains the real node weights.
--
-- @param self AnimationSpec.
-- @return Node weights, or nil.
AnimationSpec.get_node_weights = function(self)
	if not self.node_weights then return end
	local w = {}
	for k,v in pairs(self.node_weights) do
		local replace = bone_chain_mapping[k]
		if replace then
			for k1,v1 in ipairs(replace) do w[v1] = v end
		else
			w[k] = v
		end
	end
	for k in pairs(bone_chain_mapping) do
		w[k] = nil
	end
	return w
end

return AnimationSpec

require(Mod.path .. "spec")

Animationspec = Class(Spec)
Animationspec.type = "animation"
Animationspec.dict_id = {}
Animationspec.dict_cat = {}
Animationspec.dict_name = {}
Animationspec.introspect = Introspect{
	name = "Animationspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "animation", type = "string", description = "Animation name in the skeleton file."},
		{name = "channel", type = "number", description = "Channel number.", details = {integer = true, min = 0, max = 255}},
		{name = "fade_in", type = "number", description = "Fade in duration."},
		{name = "fade_out", type = "number", description = "Fade out duration."},
		{name = "permanent", type = "boolean", description = "Enables looping of the animation."},
		{name = "repeat_end", type = "number", description = "Repeat range end offset."},
		{name = "repeat_start", type = "number", description = "Repeat range start offset."},
		{name = "time", type = "number", description = "Starting time offset."},
		{name = "time_scale", type = "number", description = "Time scale multiplier."},
		{name = "weight", type = "number", description = "Blending weight."},
		{name = "node_weights", type = "dict", dict = {type = "number"}, description = "Blending weight for individual bones."},
	}}

--- Creates a new animation spec.
-- @param clss Animationspec class.
-- @param args Arguments.
-- @return Animation spec.
Animationspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets the animation playback arguments of the animation.
-- @param self Animation spec.
-- @return Table of playback arguments.
Animationspec.get_arguments = function(self)
	return {
		animation = self.animation,
		channel = self.channel,
		fade_in = self.fade_in,
		fade_out = self.fade_out,
		permanent = self.permanent,
		repeat_end = self.repeat_end,
		repeat_start = self.repeat_start,
		time = self.time,
		time_scale = self.time_scale,
		weight = self.weight,
		node_weights = self:get_node_weights()}
end

--- Gets the node weights of the animation.
--
-- The node weight dictionary may contain some helper weights that set multiple
-- real weights at once. If one of the recognized uppercase node names is seen
-- in the node weight list, it's replaced by this function. The returned table
-- contains the real node weights.
--
-- @param self Animation spec.
-- @return Node weights, or nil.
Animationspec.get_node_weights = function(self)
	if not self.node_weights then return end
	local mapping = {
		LOWER = {"back1", "pelvis", "leg1.L", "leg2.L", "foot.L", "leg1.R", "leg2.R", "foot.R", "pelvis1", "leg1", "leg2", "leg3", "leg4", "leg5", "leg6"},
		LEGS = {"leg1.L", "leg2.L", "foot.L", "leg1.R", "leg2.R", "foot.R", "leg1", "leg2", "leg3", "leg4", "leg5", "leg6"},
		ARMS = {"arm1.L", "arm2.L", "palm.L", "palm1.L", "arm1.R", "arm2.R", "palm.R", "palm1.R"},
		ARML = {"arm1.L", "arm2.L", "palm.L", "palm1.L"},
		ARMR = {"arm1.R", "arm2.R", "palm.R", "palm1.R"},
		BACK = {"back1", "back2", "back3"}}
	local w = {}
	for k,v in pairs(self.node_weights) do
		local replace = mapping[k]
		if replace then
			for k1,v1 in ipairs(replace) do w[v1] = v end
		else
			w[k] = v
		end
	end
	for k in pairs(mapping) do
		w[k] = nil
	end
	return w
end

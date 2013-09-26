--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.animation
-- @alias Animation

local Class = require("system/class")

if not Los.program_load_extension("animation") then
	error("loading extension `animation' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Animation
local Animation = Class("Animation")
Animation.dict_name = setmetatable({}, {__mode = "v"})

--- Creates an empty animation.
-- @param clss Animation class.
-- @param name Animation name.
-- @return New animation.
Animation.new = function(clss, name)
	local self = Class.new(clss)
	self.handle = Los.animation_new(name)
	if not self.handle then return end
	return self
end

--- Copies an animation.
-- @param clss Animation class.
-- @param anim Animation.
-- @return New animation.
Animation.copy = function(clss, anim)
	local self = Class.new(clss)
	self.handle = Los.animation_copy(anim.handle)
	if not self.handle then return end
	return self
end

--- Loads an animation.
-- @param clss Animation class.
-- @param name Filename.
-- @return New animation.
Animation.load = function(clss, name)
	local self = Class.new(clss)
	self.handle = Los.animation_load(name)
	if not self.handle then return end
	clss.dict_name[name] = self
	return self
end

--- Sets the transformation of a node in the given frame.
-- @param self Animation.
-- @param args Arguments.<ul>
--   <li>frame: Frame number.</li>
--   <li>node: Node name.</li>
--   <li>position: Position change relative to rest pose.</li>
--   <li>rotation: Rotation change relative to rest pose.</li>
--   <li>scale: Scale factor.</li></ul>
Animation.set_transform = function(self, args)
	local p = args.position and args.position.handle
	local r = args.rotation and args.rotation.handle
	local a = {frame = args.frame, node = args.node, position = p, rotation = r, scale = args.scale}
	return Los.animation_set_transform(self.handle, a)
end

--- Gets the duration of the animation in seconds.
-- @param self Animation.
-- @return Duration in seconds.
Animation.get_duration = function(self)
	return Los.animation_get_duration(self.handle)
end

--- Gets the number of frames in the animation
-- @param self Animation.
-- @return Number of frames.
Animation.get_frames = function(self)
	return Los.animation_get_frames(self.handle)
end

--- Gets the name of the animation.
-- @param self Animation.
-- @return Name.
Animation.get_name = function(self)
	return Los.animation_get_name(self.handle)
end

return Animation



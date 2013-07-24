--- Music specification.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.music
-- @alias MusicSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- Music specification.
-- @type MusicSpec
local MusicSpec = Spec:register("MusicSpec", "music", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "fade_in", type = "number", default = 2, description = "Fade in duration."},
	{name = "fade_out", type = "number", default = 2, description = "Fade out duration."},
	{name = "file", type = "string", description = "Music filename."},
	{name = "volume", type = "number", default = 1, description = "Volume level of the music track."}
})

--- Registers a new music spec.
-- @param clss MusicSpec class.
-- @param args Arguments.
-- @return MusicSpec.
MusicSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

return MusicSpec

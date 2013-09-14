--- Specifies a script generated or altered model.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.model
-- @alias ModelSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- Specifies a script generated or altered model.
-- @type ModelSpec
local ModelSpec = Spec:register("ModelSpec", "model")

--- Creates a new model specification.
-- @param clss ModelSpec class.
-- @param args Arguments.<ul>
--   <li>edit_materials: Dictionary of material edits.</li></ul>
--   <li>name: Name of the model.</li>
--   <li>replace_shaders: Dictionary of shaders overrides.</li></ul>
-- @return New model specification.
ModelSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

return ModelSpec

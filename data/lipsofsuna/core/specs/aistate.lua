--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.aistate
-- @alias AiStateSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- TODO:doc
-- @type AiStateSpec
local AiStateSpec = Spec:register("AiStateSpec", "ai state")

--- Registers an AI state.
-- @param clss AiStateSpec class.
-- @param args Arguments.<ul>
--   <li>calculate: Function for calculating the likelihood.</li>
--   <li>categories: List of categories.</li>
--   <li>name: State name.</li>
--   <li>update: Function for updating the AI during the state.</li></ul>
-- @return New AI state spec.
AiStateSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

return AiStateSpec

--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.aiaction
-- @alias AiActionSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- TODO:doc
-- @type AiActionSpec
local AiActionSpec = Spec:register("AiActionSpec", "ai action")

--- Registers an AI action.
-- @param clss AiActionSpec class.
-- @param args Arguments.<ul>
--   <li>calculate: Function for calculating the likelihood.</li>
--   <li>categories: List of categories.</li>
--   <li>name: Action name.</li>
--   <li>perform: Function for performing the action.</li></ul>
-- @return New AI action spec.
AiActionSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

return AiActionSpec

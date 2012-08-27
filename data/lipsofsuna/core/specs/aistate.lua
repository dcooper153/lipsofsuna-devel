--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.aistate
-- @alias Aistatespec

local Class = require("system/class")
require(Mod.path .. "spec")

--- TODO:doc
-- @type Aistatespec
Aistatespec = Class("Aistatespec", Spec)
Aistatespec.type = "aistate"
Aistatespec.dict_id = {}
Aistatespec.dict_cat = {}
Aistatespec.dict_name = {}

--- Registers an AI state.
-- @param clss Aistatespec class.
-- @param args Arguments.<ul>
--   <li>calculate: Function for calculating the likelihood.</li>
--   <li>categories: List of categories.</li>
--   <li>name: State name.</li>
--   <li>update: Function for updating the AI during the state.</li></ul>
-- @return New AI state spec.
Aistatespec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end



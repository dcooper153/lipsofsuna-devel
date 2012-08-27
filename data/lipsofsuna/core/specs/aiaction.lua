--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.aiaction
-- @alias Aiactionspec

local Class = require("system/class")
require(Mod.path .. "spec")

--- TODO:doc
-- @type Aiactionspec
Aiactionspec = Class("Aiactionspec", Spec)
Aiactionspec.type = "aiaction"
Aiactionspec.dict_id = {}
Aiactionspec.dict_cat = {}
Aiactionspec.dict_name = {}

--- Registers an AI action.
-- @param clss Aiactionspec class.
-- @param args Arguments.<ul>
--   <li>calculate: Function for calculating the likelihood.</li>
--   <li>categories: List of categories.</li>
--   <li>name: Action name.</li>
--   <li>perform: Function for performing the action.</li></ul>
-- @return New AI action spec.
Aiactionspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end



local Class = require("system/class")
require(Mod.path .. "spec")

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

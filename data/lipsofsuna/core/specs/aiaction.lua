local Class = require("system/class")
require(Mod.path .. "spec")

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

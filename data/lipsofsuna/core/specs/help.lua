require(Mod.path .. "spec")

Helpspec = Class(Spec)
Helpspec.type = "helpspec"
Helpspec.dict_id = {}
Helpspec.dict_cat = {}
Helpspec.dict_name = {}

--- Registers a help specifification.
-- @param clss Helpspec class.
-- @param args Arguments.<ul>
--   <li>name: Help topic name.</li>
--   <li>text: Help text.</li>
--   <li>title: Help page title.</li></ul>
-- @return New skill spec.
Helpspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	if not self.requires then self.requires = {} end
	return self
end

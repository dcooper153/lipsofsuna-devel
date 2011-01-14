require "common/spec"

Regionspec = Class(Spec)
Regionspec.type = "region"
Regionspec.dict_id = {}
Regionspec.dict_cat = {}
Regionspec.dict_name = {}

--- Creates a new region specification.
-- @param clss Regionspec class.
-- @param args Arguments.<ul>
--   <li>links: Array of names of connected regions.</li>
--   <li>name: Unique region name.</li>
--   <li>size: Region size in tiles.</li></ul>
-- @return New regionspec.
Regionspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.links = self.links or {}
	self.size = self.size or {4,4,4}
	return self
end

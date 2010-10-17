Regionspec = Class()
Regionspec.dict_name = {}

--- Finds a region specification.
-- @param clss Regionspec class.
-- @param args Arguments.<ul>
--   <li>name: Region name.</li></ul>
-- @return Regionspec or nil.
Regionspec.find = function(clss, args)
	return clss.dict_name[args.name]
end

--- Creates a new region specification.
-- @param clss Regionspec class.
-- @param args Arguments.<ul>
--   <li>links: Array of connectivity rules.</li>
--   <li>name: Unique region name.</li>
--   <li>size: Region size in tiles.</li>
--   <li>style: Interior style.</li></ul>
-- @return New regionspec.
Regionspec.new = function(clss, args)
	local self = Class.new(clss, args)
	self.links = self.links or {}
	self.size = self.size or {4,4,4}
	self.style = self.style or "default"
	clss.dict_name[args.name] = self
	return self
end

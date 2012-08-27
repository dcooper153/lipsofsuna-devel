local Material = require("system/material")
require "system/tiles"

Material.dict_id = {}
Material.dict_name = {}
local instfunc = Material.new

--- Finds a material
-- @param clss Model class.
-- @param args Arguments.<ul>
--   <li>id: Material ID.</li>
--   <li>name: Material name.</li></ul>
-- @return Model or nil.
Material.find = function(clss, args)
	if args.id then
		return clss.dict_id[args.id]
	end
	if args.name then
		return clss.dict_name[args.name]
	end
end

--- Creates a new material.
-- @param clss Material class.
-- @param args Arguments.
-- @return New models.
Material.new = function(clss, args)
	local self = instfunc(clss, args)
	self.magma = args.magma
	clss.dict_id[self:get_id()] = self
	clss.dict_name[self:get_name()] = self
	return self
end

--- Returns true if the material is magma.
-- @param self Material.
-- @return Boolean.
Material.get_magma = function(self)
	return self.magma
end

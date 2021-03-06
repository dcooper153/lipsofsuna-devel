--- Manages specs.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.main.spec_manager
-- @alias SpecManager

local Class = require("system/class")
local File = require("system/file")
local Spec = require("core/specs/spec")

--- Manages specs.
-- @type SpecManager
local SpecManager = Class("SpecManager")

--- Creates a new spec manager.
-- @param clss SpecManager class.
-- @return SpecManager.
SpecManager.new = function(clss)
	local self = Class.new(clss)
	self.__classes = {}
	return self
end

--- Finds specs by category.
-- @param self SpecManager.
-- @param clss Class name.
-- @param name String.
-- @return Table of specs.
SpecManager.find_by_category = function(self, clss, name)
	local specs = self.__classes[clss]
	if not specs then return end
	return specs.dict_cat[name] or {}
end

--- Finds a spec by ID.
-- @param self SpecManager.
-- @param clss Class name.
-- @param id ID.
-- @return Spec, or nil if not found.
SpecManager.find_by_id = function(self, clss, id)
	local specs = self.__classes[clss]
	if not specs then return end
	return specs.dict_id[id]
end

--- Finds a spec by name.
-- @param self SpecManager.
-- @param clss Class name.
-- @param name String.
-- @return Spec, or nil if not found.
SpecManager.find_by_name = function(self, clss, name)
	local specs = self.__classes[clss]
	if not specs then return end
	return specs.dict_name[name]
end

--- Finds a spec class by name.
-- @param self SpecManager.
-- @param clss Class name.
-- @return Spec class if found. Nil otherwise.
SpecManager.find_class = function(self, clss)
	return self.__classes[clss]
end

--- Returns a random spec.
-- @param self SpecManager.
-- @param clss Class name.
-- @return Spec if found. Nil otheriwse.
SpecManager.find_random = function(self, clss)
	local specs = self.__classes[clss]
	if not specs then return end
	return specs.dict_id[math.random(#specs.dict_id)]
end

--- Returns a random spec.
-- @param self SpecManager.
-- @param clss Class name.
-- @param category Category name.
-- @return Spec if found. Nil otheriwse.
SpecManager.find_random_by_category = function(self, clss, category)
	local specs = self.__classes[clss]
	if not specs then return end
	local cat = specs.dict_cat[category]
	if not cat then return end
	return cat[math.random(#cat)]
end

--- Registers a spec class.
-- @param self SpecManager.
-- @param name Spec name.
-- @param type Spec type.
-- @param fields Introspection fields.
-- @return New spec class.
SpecManager.register = function(self, name, type, fields)
	local spec = Class(name, Spec)
	spec.type = type
	spec.dict_id = {}
	spec.dict_cat = {}
	spec.dict_name = {}
	if fields then
		local Introspect = require("core/introspect/introspect")
		spec.introspect = Introspect{name = name, fields = fields}
	end
	self.__classes[name] = spec
	return spec
end

--- Registers all the spec types.
-- @param self SpecManager.
SpecManager.register_all = function(self)
	if not self.__registered then
		self.__registered = true
		File:require_directory("core/specs", "init")
	end
end

--- Gets the dictionary of spec names.
-- @param self SpecManager.
-- @param clss Spec class name.
-- @return Dictionary whose keys correspond to spec names.
SpecManager.get_spec_names = function(self, clss)
	local specs = self.__classes[clss]
	if not specs then return {} end
	return specs.dict_name
end

return SpecManager

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

--- Finds a spec.
-- @param self SpecManager.
-- @param clss Class name.
-- @param args Arguments.<ul>
--   <li>category: Category name.</li>
--   <li>name: Spec name.</li>
--   <li>id: Spec ID.</li></ul>
-- @return Spec or nil if searching by name or ID. Table or nil otherwise.
SpecManager.find = function(self, clss, args)
	if not args then return end
	local specs = self.__classes[clss]
	if not specs then return end
	if args.category then
		return specs.dict_cat[args.category]
	end
	if args.name then
		return specs.dict_name[args.name]
	end
	if args.id then
		return specs.dict_id[args.id]
	end
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
-- @return Dictionary whose keys correspond to spec names.
SpecManager.get_spec_names = function(self)
	return self.__classes
end

return SpecManager

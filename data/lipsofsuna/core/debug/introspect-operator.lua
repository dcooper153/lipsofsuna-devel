--- Manages the state of the introspection UI.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.debug.introspect_operator
-- @alias IntrospectOperator

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- Manages the state of the introspection UI.
-- @type IntrospectOperator
local IntrospectOperator = Class("IntrospectOperator")

--- Initializes the introspection UI.
-- @param self Operator.
IntrospectOperator.init = function(self)
	self.__spec_type = nil
	self.__spec_name = nil
	self.__field_name = nil
end

--- Gets the introspected field.
--
-- Context: The field name must have been set.
--
-- @param self Operator.
-- @param Introspecter field.
IntrospectOperator.get_field = function(self)
	local spec = self:get_spec()
	return spec.introspect.fields_dict[self.__field_name]
end

--- Gets the name of the introspected field.
--
-- Context: The field name must have been set.
--
-- @param self Operator.
-- @param String.
IntrospectOperator.get_field_name = function(self)
	return self.__field_name
end

--- Sets the name of the introspected field.
--
-- Context: The spec name must have been set.
--
-- @param self Operator.
-- @param value Field name.
IntrospectOperator.set_field_name = function(self, value)
	self.__field_name = value
end

--- Gets the fields of the introspected spec.
--
-- Context: The spec name must have been set.
--
-- @param self Operator.
-- @param List of introspecter fields.
IntrospectOperator.get_fields = function(self)
	local spec = self:get_spec()
	return spec.introspect.fields_list
end

--- Gets the introspected spec.
--
-- Context: The spec name must have been set.
--
-- @param self Operator.
-- @param Spec.
IntrospectOperator.get_spec = function(self)
	local spec_clss = Spec.dict_spec[self.__spec_type]
	return spec_clss:find{name = self.__spec_name}
end

--- Gets the list of introspectable specs.
--
-- Context: The spec type must have been set.
--
-- @param self Operator.
-- @param List of specs.
IntrospectOperator.get_specs = function(self)
	local lst = {}
	local spec_clss = Spec.dict_spec[self.__spec_type]
	for k,v in pairs(spec_clss.dict_name) do table.insert(lst, v) end
	table.sort(lst, function(a,b) return a.name < b.name end)
	return lst
end

--- Gets the name of the introspected spec.
--
-- Context: The spec name must have been set.
--
-- @param self Operator.
-- @return String.
IntrospectOperator.get_spec_name = function(self)
	return self.__spec_name
end

--- Sets the name of the introspected spec.
--
-- Context: The spec type must have been set.
--
-- @param self Operator.
-- @param value Spec name.
IntrospectOperator.set_spec_name = function(self, value)
	self.__spec_name = value
end

--- Gets the type of the introspected spec.
--
-- Context: The spec type must have been set.
--
-- @param self Operator.
-- @return String.
IntrospectOperator.get_spec_type = function(self)
	return self.__spec_type
end

--- Sets the type of the introspected spec.
--
-- Context: The introspection tool must have been initialized.
--
-- @param self Operator.
-- @param value Spec type.
IntrospectOperator.set_spec_type = function(self, value)
	self.__spec_type = value
end

return IntrospectOperator

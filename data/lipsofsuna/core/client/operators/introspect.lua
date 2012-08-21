local Class = require("system/class")

Operators.introspect = Class("IntrospectOperator")
Operators.introspect.data = {}

--- Initializes the introspection tool.
--
-- Context: Any.
--
-- @param self Operator.
Operators.introspect.init = function(self)
	self.data.spec_type = nil
	self.data.spec_name = nil
	self.data.field_name = nil
end

--- Gets the introspected field.
--
-- Context: The field name must have been set.
--
-- @param self Operator.
-- @param Introspecter field.
Operators.introspect.get_field = function(self)
	local spec = self:get_spec()
	return spec.introspect.fields_dict[self.data.field_name]
end

--- Gets the name of the introspected field.
--
-- Context: The field name must have been set.
--
-- @param self Operator.
-- @param String.
Operators.introspect.get_field_name = function(self)
	return self.data.field_name
end

--- Sets the name of the introspected field.
--
-- Context: The spec name must have been set.
--
-- @param self Operator.
-- @param value Field name.
Operators.introspect.set_field_name = function(self, value)
	self.data.field_name = value
end

--- Gets the fields of the introspected spec.
--
-- Context: The spec name must have been set.
--
-- @param self Operator.
-- @param List of introspecter fields.
Operators.introspect.get_fields = function(self)
	local spec = self:get_spec()
	return spec.introspect.fields_list
end

--- Gets the introspected spec.
--
-- Context: The spec name must have been set.
--
-- @param self Operator.
-- @param Spec.
Operators.introspect.get_spec = function(self)
	local spec_clss = _G[self.data.spec_type]
	return spec_clss:find{name = self.data.spec_name}
end

--- Gets the list of introspectable specs.
--
-- Context: The spec type must have been set.
--
-- @param self Operator.
-- @param List of specs.
Operators.introspect.get_specs = function(self)
	local lst = {}
	local spec_clss = _G[self.data.spec_type]
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
Operators.introspect.get_spec_name = function(self)
	return self.data.spec_name
end

--- Sets the name of the introspected spec.
--
-- Context: The spec type must have been set.
--
-- @param self Operator.
-- @param value Spec name.
Operators.introspect.set_spec_name = function(self, value)
	self.data.spec_name = value
end

--- Gets the type of the introspected spec.
--
-- Context: The spec type must have been set.
--
-- @param self Operator.
-- @return String.
Operators.introspect.get_spec_type = function(self)
	return self.data.spec_type
end

--- Sets the type of the introspected spec.
--
-- Context: The introspection tool must have been initialized.
--
-- @param self Operator.
-- @param value Spec type.
Operators.introspect.set_spec_type = function(self, value)
	self.data.spec_type = value
end

local Class = require("system/class")

Introspect = Class("Introspect")
Introspect.types_dict = {}

--- Creates a new introspection ruleset.
-- @param clss Introspect class.
-- @param args Arguments.<ul>
--   <li>base: Function for getting the base for inherited specs.</li>
--   <li>fields: List of introspectable fields.</li>
--   <li>name: Name string of the introspected type.</li></ul>
Introspect.new = function(clss, args)
	local self = Class.new(clss)
	self.fields_list = {}
	self.fields_dict = {}
	self.base = args.base
	self.name = args.name
	if args.fields then
		for k,v in ipairs(args.fields) do
			self:add_field(v)
		end
	end
	return self
end

--- Adds a new field to the type.
-- @param self Introspect.
-- @param args Arguments.<ul>
--   <li>default: Default value.</li>
--   <li>description: Human-readable description string.</li>
--   <li>label: Human-readable name.</li>
--   <li>name: Field name.</li>
--   <li>type: Type name.</li>
Introspect.add_field = function(self, args)
	assert(args.name)
	assert(args.type)
	assert(self.fields_dict[args.name] == nil, "duplicate introspection field " .. args.name)
	assert(self.types_dict[args.type] ~= nil, "unsupported introspection type " .. args.type)
	self.fields_dict[args.name] = args
	table.insert(self.fields_list, args)
end

--- Reads the fields of the type from a string.
-- @param self Introspect.
-- @param data Instance of the type of this introspection ruleset.
-- @param tbl Table containing fields
Introspect.read_table = function(self, data, tbl)
	-- Validate arguments.
	for k,v in pairs(tbl) do
		if not self.fields_dict[k] then
			error(string.format("type %q: invalid field %q", self.name, tostring(k)))
		end
	end
	-- Copy values from the table.
	for k,field in ipairs(self.fields_list) do
		local value = tbl and tbl[field.name]
		local type = self.types_dict[field.type]
		if value ~= nil then
			data[field.name] = value
		elseif field.default then
			data[field.name] = field.default
		end
	end
end

--- Validates the introspectable fields of the given data.
-- @param self Introspect.
-- @param data Instance of the type of this introspection ruleset.
Introspect.validate = function(self, data)
	local validate_type = function(field, value)
		local t = self.types_dict[field.type]
		assert(t)
		if t.validate and not t.validate(value, field) then
			error(type(value) .. " not of type " .. field.type, 0)
		end
	end
	local validate_details
	validate_details = function(details, value)
		-- Validate numeric constraints.
		if details.integer and value ~= math.floor(value) then
			error(tostring(value) .. " not an integer", 0)
		end
		if details.min and value < details.min then
			error(tostring(value) .. " not greater or equal to " .. tostring(details.min), 0)
		end
		if details.max and value > details.max then
			error(tostring(value) .. " not less or equal to " .. tostring(details.max), 0)
		end
		-- Validate spec relations.
		if details.spec then
			local specclss = _G[details.spec]
			assert(specclss)
			if not specclss:find{name = value} then
				error(value .. " not in " .. details.spec, 0)
			end
		end
		-- Validate tables.
		if details.keys then
			for k in pairs(value) do
				validate_details(details.keys, k)
			end
		end
		if details.values then
			for k,v in pairs(value) do
				validate_details(details.values, v)
			end
		end
	end
	for k,field in ipairs(self.fields_list) do
		local value = data[field.name]
		if value ~= nil then
			local ret,err = pcall(validate_type, field, value)
			if not ret then error(string.format("type %q, field %q: %s", self.name, field.name, err), 0) end
			if field.details then
				local ret,err = pcall(validate_details, field.details, value)
				if not ret then error(string.format("type %q, field %q: %s", self.name, field.name, err), 0) end
			end
		end
	end
end

--- Converts an instance of an introspectable type to a string.
-- @param self Introspect.
-- @param data Instance of the type of this introspection ruleset.
-- @return String.
Introspect.write_str = function(self, data)
	local str = self.name .. "{"
	local first = true
	local write = function(k, v)
		-- Get the field information.
		local type = self.types_dict[v.type]
		local value = data[v.name]
		local default = v.default
		-- Get the base spec value.
		local base = self.base and self.base(data)
		if base then default = base[v.name] end
		-- Omit default values.
		if value == nil then return end
		if default ~= nil and type.equals(value, default) then return end
		-- Convert the value to a string.
		local ret,s = xpcall(function() return type.write_str(value, v) end, function(err)
			print(string.format("ERROR: Failed to write field %q of type %q", v.name, v.type))
			print(debug.traceback("ERROR: " .. err)) end)
		if not s then return end
		-- Append the field to the output.
		if first then
			first = false
			str = string.format("%s\n", str)
		else
			str = string.format("%s,\n", str)
		end
		str = string.format("%s\t%s = %s", str, v.name, s)
	end
	for k,v in ipairs(self.fields_list) do
		write(k, v)
	end
	return str .. "}"
end

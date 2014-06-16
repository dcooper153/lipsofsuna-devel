--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.spec
-- @alias Spec

local Class = require("system/class")
local File = require("system/file")

--- TODO:doc
-- @type Spec
local Spec = Class("Spec")

--- Registers a spec class.
-- @param clss Spec class.
-- @param name Spec name.
-- @param type Spec type.
-- @param fields Introspection fields.
-- @return New spec class.
Spec.register = function(clss, name, type, fields)
	return Main.specs:register(name, type, fields)
end

--- Finds specs by category.
-- @param clss Spec class.
-- @param name String.
-- @return Table of specs.
Spec.find_by_category = function(clss, name)
	return clss.dict_cat[name] or {}
end

--- Finds a spec by ID.
-- @param clss Spec class.
-- @param id ID.
-- @return Spec, or nil if not found.
Spec.find_by_id = function(clss, id)
	return clss.dict_id[id]
end

--- Finds a spec by name.
-- @param clss Spec class.
-- @param name String.
-- @return Spec, or nil if not found.
Spec.find_by_name = function(clss, name)
	return clss.dict_name[name]
end

--- Creates a new spec.
-- @param clss Spec class.
-- @param args Arguments.<ul>
--   <li>categories: List of categories, or nil.</li>
--   <li>name: Spec name.</li></ul>
-- @return New item spec.
Spec.new = function(clss, args)
	local self = Class.new(clss)
	self.id = #clss.dict_id + 1
	for k,v in pairs(args) do self[k] = v end
	-- Setup categories.
	self.categories = args.categories or {}
	-- Register.
	clss.dict_id[self.id] = self
	clss.dict_name[self.name] = self
	for k in pairs(self.categories) do
		local cat = clss.dict_cat[k]
		if not cat then
			cat = {self}
			clss.dict_cat[k] = cat
		else
			table.insert(cat, self)
		end
	end
	-- Store the source filename.
	self.file = Program:get_calling_file(5)
	if self.file then
		local d = clss.dict_file[self.file]
		if not d then
			d = {}
			clss.dict_file[self.file] = d
		end
		table.insert(d, self)
	end
	return self
end

--- Checks if the given field is set and is not the default value.
-- @param self Spec.
-- @param name Field name.
-- @return True if set and not default.
Spec.is_field_set = function(self, name)
	-- Check if nil.
	local value = self[name]
	if value == nil then return end
	-- Find the introspection field.
	if not self.introspect then return true end
	local field = self.introspect.fields_dict[name]
	if not field then return end
	-- Check if default.
	if field.default == nil then return true end
	local Introspect = require("core/introspect/introspect")
	if not Introspect:equals(field.type, value, field.default) then return true end
end

--- Returns a random spec.
-- @param clss Spec class.
-- @param args Arguments.<ul>
--   <li>category: Category name or nil.</li></ul>
-- @return ObstacleSpec or nil.
Spec.random = function(clss, args)
	if args and args.name then
		return clss.dict_name[args.name]
	elseif args and args.category then
		local cat = clss.dict_cat[args.category]
		if not cat then return end
		return cat[math.random(#cat)]
	end
	return clss.dict_id[math.random(#clss.dict_id)]
end

--- Validates the spec.
-- @param self Spec.
Spec.validate = function(self)
	if self.introspect then
		xpcall(function() self.introspect:validate(self) end,
			function(err) print(string.format("ERROR: In spec %q, %s", self.name, err)) end)
	end
end

--- Validates all specs.
-- @param clss Spec class.
Spec.validate_all = function(clss)
	for name,spec in pairs(clss.dict_name) do
		spec:validate()
	end
end

--- Writes the spec to its source file.<br/>
--
-- This function loads the source Lua file of the spec to a string and attempts
-- to manually parse it. If the a substring matching the spec is found from the
-- string, it is replaced with the serialized version of the spec.<br/>
--
-- Upon success, the Lua source file containing the spec is rewritten so that
-- the new spec replaces the old one. True is returned in such a case. If the
-- spec could not be found from the file, false is returned.<br/>
--
-- NOTE: This function does not have perfect parsing at the moment. Known failures
-- include mistaking commented out specs as the real one, and not detecting the
-- spec if is defined with an alias or if the name isn't the first field.
--
-- @param self Spec.
-- @return True on success.
Spec.write_file = function(self)
	if not self.introspect then return end
	-- Read the file constaining the spec.
	local f = File:read(self.file)
	if not f then return end
	-- Find the start of the spec.
	local p = string.format("%s{[ \n\t]*name[ \n\t]*=[ \n\t]*%q", self.introspect.name, self.name)
	local m = string.find(f, p)
	if not m then return end
	-- Find the end of the spec.
	local i = m
	local stack = 0
	local length = string.len(f)
	local skip = function()
		-- Skip multiline comments.
		if string.sub(f, i, i + 3) == "--[[" then
			i = i + 4
			while i <= length and string.sub(f, i, i + 1) ~= "]]" do
				i = i + 1
			end
		-- Skip multiline strings.
		elseif string.sub(f, i, i + 1) == "[[" then
			i = i + 2
			while i <= length and string.sub(f, i, i + 1) ~= "]]" do
				i = i + 1
			end
		-- Skip strings.
		elseif f[i] == "\"" then
			i = i + 1
			while i <= length do
				local c = string.sub(f, i, i)
				if c == "\\" then
					i = i + 2
				elseif c ~= "\"" then
					i = i + 1
				else
					break
				end
			end
		elseif f[i] == "\'" then
			i = i + 1
			while i <= length do
				local c = string.sub(f, i, i)
				if c == "\\" then
					i = i + 2
				elseif c ~= "'" then
					i = i + 1
				else
					break
				end
			end
		-- Nothing skipped.
		else
			return true
		end
	end
	local success
	while i < length do
		if skip() then
			local c = string.sub(f, i, i)
			if c == "{" then
				stack = stack + 1
			elseif c == "}" then
				stack = stack - 1
				if stack == 0 then
					success = true
					break
				end
			end
			i = i + 1
		end
	end
	-- Make sure that the spec was correctly terminated.
	if not success then return end
	-- Replace the old spec string with a new one.
	local new = self:write_str()
	if not new then return end
	local str = string.sub(f, 1, m - 1) .. new .. string.sub(f, i + 1, length)
	-- Overwrite the old file with the new string.
	File:write(self.file, str)
	return true
end

--- Writes the spec to a new file.<br/>
--
-- Upon success, a new file is created and the spec is saved to it and the
-- source file of the spec is set to that file.
--
-- @param self Spec.
-- @param file Filename.
-- @return True on success.
Spec.write_file_new = function(self, file)
	if not self.introspect then return end
	local new = self:write_str()
	if not new then return end
	if not File:write(file, new .. "\n") then return end
	self.file = file
	return true
end

--- Writes the spec to a string.
-- @param self Spec.
-- @return String, or none if not implemented.
Spec.write_str = function(self)
	if self.introspect then
		return self.introspect:write_str(self)
	end
end

return Spec

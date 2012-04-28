Spec = Class()
Spec.dict_id = {}
Spec.dict_cat = {}
Spec.dict_name = {}
Spec.dict_file = {}

--- Extends a spec with the values in the table.
-- @param clss Spec class.
-- @param args Arguments.<ul>
--   <li>name: Spec name.</li></ul>
Spec.extend = function(clss, args)
	local spec = clss:find(args)
	if not spec then return end
	for k,v in pairs(args) do
		if k ~= "name" then spec[k] = v end
	end
end

--- Finds a spec.
-- @param clss Spec class.
-- @param args Arguments.<ul>
--   <li>category: Category name.</li>
--   <li>name: Spec name.</li>
--   <li>id: Spec ID.</li></ul>
-- @return Spec or nil if searching by name or ID. Table or nil otherwise.
Spec.find = function(clss, args)
	if not args then return end
	if args.category then
		return clss.dict_cat[args.category]
	end
	if args.name then
		return clss.dict_name[args.name]
	end
	if args.id then
		return clss.dict_id[args.id]
	end
end

--- Creates a new spec.
-- @param clss Spec class.
-- @param args Arguments.<ul>
--   <li>categories: List of categories, or nil.</li>
--   <li>name: Spec name.</li></ul>
-- @return New item spec.
Spec.new = function(clss, args)
	local self = Class.new(clss, args)
	self.id = #clss.dict_id + 1
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

--- Returns a random spec.
-- @param clss Spec class.
-- @param args Arguments.<ul>
--   <li>category: Category name or nil.</li></ul>
-- @return Obstaclespec or nil.
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

--- Writes the spec to a string.
-- @param self Spec.
-- @return String, or none if not implemented.
Spec.write_str = function(self)
	if self.introspect then
		return self.introspect:write_str(self)
	end
end

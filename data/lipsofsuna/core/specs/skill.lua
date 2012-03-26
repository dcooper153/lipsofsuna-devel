require(Mod.path .. "spec")

Skillspec = Class(Spec)
Skillspec.type = "skillspec"
Skillspec.dict_id = {}
Skillspec.dict_cat = {}
Skillspec.dict_name = {}

--- Registers a skill specifification.
-- @param clss Skillspec class.
-- @param args Arguments.<ul>
--   <li>assign: Function that is called when the skill is assigned.</li>
--   <li>description: Description string.</li>
--   <li>icon: Icon name.</li>
--   <li>name: Skill name.</li>
--   <li>requires: Table of required skills.</li></ul>
-- @return New skill spec.
Skillspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	if not self.requires then self.requires = {} end
	return self
end

--- Finds all skills that are directly dependent on this one.
-- @param self Skillspec.
-- @return Array of specs.
Skillspec.find_directly_dependent = function(self)
	local deps = {}
	for name,spec in pairs(self.dict_name) do
		for index,requires in pairs(spec.requires) do
			if self.name == requires then
				deps[name] = spec
				break
			end
		end
	end
	return deps
end

--- Finds all skills on which this one depends directly.
-- @param self Skillspec.
-- @return Table of specs.
Skillspec.find_direct_requirements = function(self)
	local reqs = {}
	for index,name in pairs(self.requires) do
		local spec = Skillspec:find{name = name}
		if spec and not reqs[name] then
			reqs[name] = spec
		end
	end
	return reqs
end

--- Finds all skills that are directly or indirectly dependent on this one.
-- @param self Skillspec.
-- @return Table of specs.
Skillspec.find_indirectly_dependent = function(self)
	-- Set this spec as a dependency.
	local more = true
	local deps = {[self.name] = self}
	-- Repeat until no more dependencies are found.
	while more do
		more = false
		-- Loop through the specs not yet in the dependency list.
		for name,spec in pairs(self.dict_name) do
			if not deps[name] then
				-- Check if the spec depends on the dependencies.
				for index,requires in pairs(spec.requires) do
					if deps[requires] then
						-- Add the spec to the dependency list.
						-- Since the list increased, we need to repeat the process.
						deps[name] = spec
						more = true
						break
					end
				end
			end
		end
	end
	-- Remove this spec from the list.
	deps[self.name] = nil
	return deps
end

--- Finds all skills on which this one depends directly or indirectly.
-- @param self Skillspec.
-- @return Table of specs.
Skillspec.find_indirect_requirements = function(self)
	local reqs = {}
	local recurse
	recurse = function(spec)
		for index,name in pairs(spec.requires) do
			local spec = Skillspec:find{name = name}
			if spec and not reqs[name] then
				reqs[name] = spec
				recurse(spec)
			end
		end
	end
	recurse(self)
	return reqs
end

--- Mod loading.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module common.mod
-- @alias Mod

local Class = require("system/class")
local File = require("system/file")
local Json = require("system/json")

--- Mod loading.
-- @type Mod
local Mod = Class("Mod")

--- Creates new mod loader.
-- @param clss Mod class.
-- @return Mod.
Mod.new = function(clss, mods)
	local self = Class.new(clss)
	self.launchers = {}
	self.mods = {}
	self.options = {}
	self.resources = {}
	self.scripts = {}
	self.specs = {}
	return self
end

--- Initializes all the loaded mods.
-- @param self Mod.
Mod.init_all = function(self, settings)
	local needed = function(name)
		local m = self.mods[name]
		if settings.server and m.server == false then return end
		if settings.client and m.client == false then return end
		return true
	end
	-- Add the resource paths.
	if self.resources then
		for k,v in ipairs(self.resources) do
			if needed(v[1]) then
				Program:add_path(v[2])
			end
		end
	end
	-- Run the scripts.
	for k,v in ipairs(self.scripts) do
		if needed(v[1]) then
			local prev_name = self.name
			local prev_path = self.path
			self.name = v[1]
			self.path = v[2]
			require(v[3])
			self.name = prev_name
			self.path = prev_path
		end
	end
	-- Create the specs.
	for k,v in ipairs(self.specs) do
		if needed(v[1]) then
			-- FIXME: Should not accept just any global.
			local cls = _G[v[3]]
			if not cls then
				error("loading mod \"" .. v[1] .. "\" failed: " ..
					"invalid spec type \"" .. v[3] .. "\" in \"" .. v[2] .. "\"")
			end
			cls(v[4])
		end
	end
end

--- Loads a mod by name.
-- @param self Mod.
-- @param name Mod name.
-- @param optional True for optional.
Mod.load = function(self, name, optional)
	local load_spec = function(info, file)
		-- Open the file.
		local path = name .. "/" .. file .. ".json"
		local specs = File:read(path)
		if not specs then
			return "could not open specs \"" .. path .. "\""
		end
		-- Decode the file.
		local res,err = Json:decode(specs)
		if err then
			return "could not load specs \"" .. path .. "\": " .. err
		end
		specs = res
		-- Create each spec.
		for k,spec in ipairs(specs) do
			-- Get the type.
			local type_ = spec["type"]
			if not type_ then
				return "missing spec type in \"" .. path .. "\""
			end
			-- Validate the type.
			-- FIXME: Should not accept just any type.
			spec["type"] = nil
			table.insert(self.specs, {name, path, type_, spec})
		end
	end

	-- Do not load multiple times.
	if self.mods[name] then return end
	-- Load the info.
	local info = File:read(name .. "/__mod__.json")
	if info then
		local res,err = Json:decode(info)
		if err then
			error("loading mod \"" .. name .. "\" failed: " .. err)
		end
		info = res
	else
		if optional then return end
		info = {scripts = {"init"}} -- Backwards compatibility.
	end
	self.mods[name] = info
	-- Collect resource paths.
	if info.resources then
		for k,v in ipairs(info.resources) do
			table.insert(self.resources, {name, name .. "/" .. v})
		end
	end
	-- Collect launchers.
	if info.launchers then
		for k,v in ipairs(info.launchers) do
			v["mod"] = name
			table.insert(self.launchers, v)
		end
	end
	-- Collect options.
	if info.options then
		for k,v in ipairs(info.options) do
			v["mod"] = name
			table.insert(self.options, v)
		end
	end
	-- Collect specs.
	if info.specs then
		for k,v in ipairs(info.specs) do
			local err = load_spec(info, v)
			if err then
				error("loading mod \"" .. name .. "\" failed: " .. err)
			end
		end
	end
	-- Collect scripts.
	if info.scripts then
		local path = name .. "/"
		for k,v in ipairs(info.scripts) do
			table.insert(self.scripts, {name, path, name .. "/" .. v})
		end
	end
end

--- Loads a list of mods from a JSON file.
-- @param self Mod.
-- @param file Filename.
Mod.load_list = function(self, file)
	-- Open the file.
	local info = File:read(file)
	if not info then
		error("opening mod list \"" .. file .. "\" failed")
	end
	-- Parse the file.
	local res,err = Json:decode(info)
	if err then
		error("loading mod list \"" .. file .. "\" failed: " .. err)
	end
	-- Load the listed mods.
	for k,v in ipairs(res) do
		if type(v) == "table" then
			if v[1] == "load" then
				self:load(v[2])
			elseif v[1] == "load_list" then
				self:load_list(v[2] .. ".json")
			elseif v[1] == "load_optional" then
				self:load(v[2], true)
			end
		end
	end
end

return Mod

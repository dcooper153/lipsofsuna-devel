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
Mod.new = function(clss)
	local self = Class.new(clss)
	self.launchers = {}
	self.mods = {}
	self.options = {}
	self.resources = {}
	self.scripts = {}
	self.post_scripts = {}
	self.specs = {}
	return self
end

--- Initializes all the loaded mods.
-- @param self Mod.
-- @param settings Settings.
Mod.init_all = function(self, settings)
	local needed = function(name)
		local m = self.mods[name]
		if settings.server and m.server == false then return end
		if settings.client and m.client == false then return end
		return true
	end
	local needed_script = function(name, mode)
		if not needed(name) then return end
		if not mode then return true end
		if mode == "server" and settings.server then return true end
		if mode == "client" and settings.client then return true end
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
		if needed_script(v[1], v[4]) then
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
			local cls = Main.specs:find_class(v[3])
			if not cls then
				error("loading mod \"" .. v[1] .. "\" failed: " ..
					"invalid spec type \"" .. v[3] .. "\" in \"" .. v[2] .. "\"")
			end
			cls(v[4])
		end
	end
	-- Run the post-spec scripts.
	for k,v in ipairs(self.post_scripts) do
		if needed_script(v[1], v[4]) then
			local prev_name = self.name
			local prev_path = self.path
			self.name = v[1]
			self.path = v[2]
			require(v[3])
			self.name = prev_name
			self.path = prev_path
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
			return path .. ": " .. err
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
			local cls = Main.specs:find_class(type_)
			if not cls then
				return "invalid spec type \"" .. type_ .. "\" in \"" .. path .. "\""
			end
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
		error("loading mod \"" .. name .. "\" failed: could not open __mod__.json")
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
				error("loading mod \"" .. name .. "\" failed:\nERROR: " .. err)
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
	if info.post_scripts then
		local path = name .. "/"
		for k,v in ipairs(info.post_scripts) do
			table.insert(self.post_scripts, {name, path, name .. "/" .. v})
		end
	end
	if info.client_scripts then
		local path = name .. "/"
		for k,v in ipairs(info.client_scripts) do
			table.insert(self.scripts, {name, path, name .. "/" .. v, "client"})
		end
	end
	if info.server_scripts then
		local path = name .. "/"
		for k,v in ipairs(info.server_scripts) do
			table.insert(self.scripts, {name, path, name .. "/" .. v, "server"})
		end
	end
	-- Load child mods.
	if info.mods then
		self:load_list_json(info.mods, true)
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
	self:load_list_json(res)
end

--- Loads a list of mods from decoded JSON data.
-- @param self Mod.
-- @param json Decoded JSON data.
Mod.load_list_json = function(self, json)
	for k,v in ipairs(json) do
		if type(v) == "table" then
			if v[1] == "load" then
				self:load(v[2], false)
			elseif v[1] == "load_list" then
				self:load_list(v[2] .. ".json")
			elseif v[1] == "load_optional" then
				self:load(v[2], true)
			end
		end
	end
end

--- Gets information on a game mode launcher.
-- @param self Mod.
-- @param name String.
-- @return Table if found. Nil otherwise.
Mod.get_launcher_info = function(self, name)
	for k,v in pairs(self.launchers) do
		if v.name == name then return v end
	end
end

return Mod

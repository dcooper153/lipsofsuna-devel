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

--- Loads a list of mods from a JSON file.
-- @param self Mod class.
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
				self:load_optional(v[2])
			end
		end
	end
end

--- Loads a mod by name.
-- @param self Mod class.
-- @param name Mod name.
Mod.load = function(self, name)
	local load_spec = function(file)
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
			local t = spec["type"]
			if not t then
				return "missing spec type in \"" .. path .. "\""
			end
			-- Validate the type.
			-- FIXME: Should not accept just any global.
			spec["type"] = nil
			local cls = _G[t]
			if not cls then
				return "invalid spec type \"" .. t .. "\" in \"" .. path .. "\""
			end
			-- Create the spec.
			cls(spec)
		end
	end

	-- Load the info.
	local info = File:read(name .. "/__mod__.json")
	if info then
		local res,err = Json:decode(info)
		if not err then
			info = res
		else
			error("loading mod \"" .. name .. "\" failed: " .. err)
		end
	else
		info = {scripts = {"init"}} -- Backwards compatiblity.
	end
	-- Check for the game mode.
	if Settings.server and info.server == false then return end
	if Settings.client and info.client == false then return end
	-- Add the resource paths.
	if info.resources then
		for k,v in ipairs(info.resources) do
			Program:add_path(name .. "/" .. v)
		end
	end
	-- Load the specs.
	if info.specs then
		for k,v in ipairs(info.specs) do
			local err = load_spec(v)
			if err then
				error("loading mod \"" .. name .. "\" failed: " .. err)
			end
		end
	end
	-- Load the scripts.
	if info.scripts then
		local prev_name = self.name
		local prev_path = self.path
		self.name = name
		self.path = name .. "/"
		for k,v in ipairs(info.scripts) do
			require(name .. "/" .. v)
		end
		self.name = prev_name
		self.path = prev_path
	end
end

Mod.load_optional = function(self, name)
	local prev_name = self.name
	local prev_path = self.path
	self.name = name
	self.path = name .. "/"
	pcall(require, name .. "/init")
	self.name = prev_name
	self.path = prev_path
end

return Mod

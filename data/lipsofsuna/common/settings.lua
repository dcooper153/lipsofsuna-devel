--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module common.settings
-- @alias Settings

local Class = require("system/class")
local Mod = require("common/mod")
local String = require("system/string")

--- TODO:doc
-- @type Settings
local Settings = Class("Settings")
Settings.arguments = String.split(Program:get_args())

--- Parses command line arguments.
-- @param clss Settings class.
Settings.parse_command_line = function(clss)
	-- Parses an individual argument.
	local parse_argument = function(mode, a, i)
		if mode == "string" then
			if i < #a then return end
			return a[i + 1],2
		elseif mode == "int" then
			if i < #a then return end
			return tonumber(a[i + 1]),2
		elseif mode == "false" then
			return false,1
		else
			return true,1
		end
	end
	-- Set default options.
	for k,v in ipairs(Mod.options) do
		if v.default ~= nil then
			clss[v.name] = v.default
		end
	end
	-- Parse arguments.
	local i = 1
	local a = clss.arguments
	while i <= #a do
		local incr = nil
		-- Try mod launchers.
		for k,v in ipairs(Mod.launchers) do
			if a[i] == v.short or a[i] == v.long then
				clss[v.name] = true
				incr = 1
			end
		end
		-- Try mod options.
		for k,v in ipairs(Mod.options) do
			if a[i] == v.short or a[i] == v.long then
				local val,inc = parse_argument(v.type, a, i)
				incr = inc
				if not incr then break end
				clss[v.name] = val
			end
		end
		-- Increment the position.
		if not incr then
			clss.help = true
			break
		end
		i = i + incr
	end
	-- Host by default.
	if not clss.client and not clss.server and not clss.editor then
		clss.client = true
	end
	-- Check for validity.
	if clss.version then return end
	if clss.help then return end
	if clss.host and clss.editor then return end
	if clss.join and clss.editor then return end
	if clss.client and clss.server then return end
	if clss.client and clss.editor then return end
	if clss.editor and clss.server then return end
	if clss.admin and not clss.host and not clss.server then return end
	if not clss.host and not clss.server and clss.generate then return end
	return true
end

--- Returns the usage string.
-- @param clss Settings class.
-- @return Usage string.
Settings.usage = function(clss)
	if clss.version then
		return Program:get_version()
	else
  		local msg = "Usage: lipsofsuna [options]\n\nOptions:\n"
		local opts = {}
		-- Add mod launchers.
		for k,v in ipairs(Mod.launchers) do
			table.insert(opts, string.format("  %-2s %-24s %s", v.short or "", v.long or "", v.desc or ""))
		end
		-- Add mod options.
		for k,v in ipairs(Mod.options) do
			if v.var then
				local long = v.long and (v.long .. " " .. v.var) or v.var
				table.insert(opts, string.format("  %-2s %-24s %s", v.short or "", long, v.desc or ""))
			else
				table.insert(opts, string.format("  %-2s %-24s %s", v.short or "", v.long or "", v.desc or ""))
			end
		end
		-- Sort the options.
		table.sort(opts)
		return msg .. table.concat(opts, "\n")
	end
end

return Settings

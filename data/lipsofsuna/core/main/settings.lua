--- Reads settings from the command line.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.main.settings
-- @alias Settings

local Class = require("system/class")
local Program = require("system/core")
local String = require("system/string")

--- Reads settings from the command line.
-- @type Settings
local Settings = Class("Settings")

--- Creates new command line settings.
-- @param clss Settings class.
-- @param mods Mod loader.
-- @return Settings.
Settings.new = function(clss, mods)
	local self = Class.new(clss)
	self.mods = mods
	self.arguments = String.split(Program:get_args())
	return self
end

--- Parses command line arguments.
-- @param self Settings.
-- @return True on success. False otherwise.
Settings.parse_command_line = function(self)
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
	for k,v in ipairs(self.mods.options) do
		if v.default ~= nil then
			self[v.name] = v.default
		end
	end
	-- Parse arguments.
	local i = 1
	local a = self.arguments
	while i <= #a do
		local incr = nil
		-- Try mod launchers.
		for k,v in ipairs(self.mods.launchers) do
			if a[i] == v.short or a[i] == v.long then
				self[v.name] = true
				incr = 1
			end
		end
		-- Try mod options.
		for k,v in ipairs(self.mods.options) do
			if a[i] == v.short or a[i] == v.long then
				local val,inc = parse_argument(v.type, a, i)
				incr = inc
				if not incr then break end
				self[v.name] = val
			end
		end
		-- Increment the position.
		if not incr then
			self.help = true
			break
		end
		i = i + incr
	end
	-- Host by default.
	if not self.client and not self.server and not self.editor then
		self.client = true
	end
	-- Check for validity.
	if self.version then return end
	if self.help then return end
	if self.host and self.editor then return end
	if self.join and self.editor then return end
	if self.client and self.server then return end
	if self.client and self.editor then return end
	if self.editor and self.server then return end
	if self.admin and not self.host and not self.server then return end
	if not self.host and not self.server and self.generate then return end
	return true
end

--- Returns the usage string.
-- @param self Settings.
-- @return Usage string.
Settings.usage = function(self)
	if self.version then
		return Program:get_version()
	else
  		local msg = "Usage: lipsofsuna [options]\n\nOptions:\n"
		local opts = {}
		-- Add mod launchers.
		for k,v in ipairs(self.mods.launchers) do
			table.insert(opts, string.format("  %-2s %-24s %s", v.short or "", v.long or "", v.desc or ""))
		end
		-- Add mod options.
		for k,v in ipairs(self.mods.options) do
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

--- Player account.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.account.account
-- @alias Account

local Class = require("system/class")

--- Player account.
-- @type Account
local Account = Class("Account")

--- Loads or creates an account.
-- @param clss Account class.
-- @param login Login name.
-- @param password Hashed password.
-- @param permissions Permission mask, or nil.
-- @param character Character string, or nil.
-- @param spawnpoint Spawn point string, or nil.
-- @return Account.
Account.new = function(clss, login, password, permissions, character, spawnpoint)
	local self = Class.new(clss)
	self.login = login
	self.password = password
	self.permissions = permissions or 0
	self.character = character
	if spawnpoint then
		local ok,vec = pcall(loadstring("return " .. spawnpoint))
		if ok then self.spawn_point = vec end
	end
	return self
end

return Account

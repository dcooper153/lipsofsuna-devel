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
-- @param client Client.
-- @param login Login name.
-- @param password Hashed password.
-- @return Account.
Account.new = function(clss, client, login, password)
	local self = Class.new(clss)
	self.client = client
	self.login = login
	self.password = password
	self.permissions = 0
	self.character = nil
	return self
end

return Account

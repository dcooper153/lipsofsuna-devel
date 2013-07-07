--- Account manager.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.account.account_manager
-- @alias AccountManager

local Account = require("core/account/account")
local Class = require("system/class") 
local Password = require("system/password")
local Serializer = require("system/serializer")

--- Account manager.
-- @type AccountManager
local AccountManager = Class("AccountManager")

--- Creates a new account database.
-- @param clss AccountManager class.
-- @param db Database.
-- @return AccountManager.
AccountManager.new = function(clss)
	local self = Class.new(clss)
	self.serializer = Serializer{}
	self.__version = "2"
	self.__salt = Password:random_salt()
	return self
end

--- Creates an empty account.
-- @param self AccountManager.
-- @param login Login name.
-- @param password Password.
-- @return Account.
AccountManager.create_account = function(self, login, password)
	local hash = self:hash_password(login, password)
	local account = Account(login, hash)
	self:save_account(account)
	return account
end

--- Creates a hashed version of the password.
-- @param self AccountManager.
-- @param login Login name.
-- @param password Password string.
-- @return Hashed password string.
AccountManager.hash_password = function(self, login, password)
	return Password:hash(password, self.__salt)
end

--- Loads or creates an account.
-- @param self AccountManager.
-- @param login Login name.
-- @param password Password.
-- @return Account or nil, status message or nil.
AccountManager.load_account = function(self, login, password)
	local hash = self:hash_password(login, password)
	local account = Account(login, hash)
	return account, "login successful"
end

--- Resets the account database.
-- @param self AccountManager.
AccountManager.reset = function(self)
	-- Initialize password hashing.
	self.__salt = Password:random_salt()
	self:set_option("password_salt", self.__salt)
	-- Set the version number.
	self:set_option("account_version", self.__version)
end

--- Strips character data from all accounts.
-- @param self AccountDatabase.
AccountManager.reset_characters = function(self)
end

--- Saves a player account.
-- @param self AccountManager.
-- @param account Account.
-- @param object Player object, or nil.
AccountManager.save_account = function(self, account, object)
end

--- Saves all active player accounts.
-- @param self AccountManager.
AccountManager.save_accounts = function(self)
	for k,v in pairs(Server.players_by_client) do
		self:save_account(v.account, v)
	end
end

--- Gets an account option.
-- @param self AccountManager.
-- @param key Option name.
-- @return Option value.
AccountManager.get_option = function(self, key)
	return self.__options[key]
end

--- Sets an account database option.
-- @param self AccountManager.
-- @param key Option name.
-- @param value Option value.
AccountManager.set_option = function(self, key, value)
	self.__options[key] = value
end

return AccountManager

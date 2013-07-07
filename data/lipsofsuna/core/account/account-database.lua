--- Account database.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.account.account_database
-- @alias AccountDatabase

local Account = require("core/account/account")
local AccountManager = require("core/account/account-manager")
local Class = require("system/class") 

--- Account database.
-- @type AccountDatabase
local AccountDatabase = Class("AccountDatabase", AccountManager)

--- Creates a new account database.
-- @param clss AccountDatabase class.
-- @param db Database.
-- @return AccountDatabase.
AccountDatabase.new = function(clss, db)
	local self = AccountManager.new(clss)
	self.__db = db
	-- Load the options.
	self.__salt = self:get_option("password_salt")
	local version = self:get_option("account_version")
	-- Check if a full reset is needed.
	if not self.__salt or version ~= self.__version then
		self:reset()
	end
	return self
end

--- Loads an account from the account database.
-- @param self AccountDatabase.
-- @param login Login name.
-- @param password Password.
-- @return Account or nil, status message or nil.
AccountDatabase.load_account = function(self, login, password)
	-- Load the account data.
	local r = self.__db:query(
		[[SELECT login,password,permissions,character,spawn_point
		FROM accounts WHERE login=?;]], {login})
	local v = r[1]
	if not v then return nil end
	-- Validate the password.
	local hash = self:hash_password(login, password)
	if v[2] ~= hash then return nil, "authentication failed" end
	-- Create the account.
	local account = Account(login, hash, v[3], v[4], v[5])
	-- Load the fields.
	local rows = self.__db:query(
		[[SELECT name,value FROM account_fields WHERE account=?]], {login})
	self.serializer:read(account, rows)
	-- Return the account and the status message.
	return account, "login successful"
end

--- Resets the account database.
-- @param self AccountDatabase.
AccountDatabase.reset = function(self)
	-- Initialize tables.
	self.__db:query([[DROP TABLE IF EXISTS accounts;]])
	self.__db:query([[DROP TABLE IF EXISTS account_fields;]])
	self.__db:query([[DROP TABLE IF EXISTS options;]])
	self.__db:query(
		[[CREATE TABLE accounts (
		login TEXT PRIMARY KEY,
		password TEXT,
		permissions INTEGER,
		character TEXT,
		spawn_point TEXT);]])
	self.__db:query([[CREATE TABLE account_fields (
		account TEXT,
		name TEXT,
		value TEXT,
		PRIMARY KEY(account,name));]])
	self.__db:query(
		[[CREATE TABLE options (
		key TEXT PRIMARY KEY,
		value TEXT);]])
	-- Reset the parent class.
	AccountManager.reset(self)
end

--- Strips character data from all accounts.
-- @param self AccountDatabase.
AccountDatabase.reset_characters = function(self)
	self.__db:query([[UPDATE accounts SET character = NULL;]])
	self.__db:query([[UPDATE accounts SET spawn_point = NULL;]])
end

--- Saves a player account.
-- @param self AccountDatabase.
-- @param account Account.
-- @param object Player object, or nil.
AccountDatabase.save_account = function(self, account, object)
	-- Save the account.
	self.__db:query(
		[[REPLACE INTO accounts
		(login,password,permissions,character,spawn_point)
		VALUES (?,?,?,?,?);]],
		{account.login, account.password, account.permissions, object and object:get_id(), account.spawn_point and tostring(account.spawn_point)})
	-- Save the fields.
	self.__db:query([[DELETE FROM account_fields WHERE account=?;]], {account.login})
	self.serializer:write(account, function(name, value)
		self.__db:query(
			[[REPLACE INTO account_fields
			(account,name,value)
			VALUES (?,?,?);]],
			{account.login, name, value})
	end)
end

--- Gets an account database option.
-- @param self AccountDatabase.
-- @param key Option name.
-- @return Option value.
AccountDatabase.get_option = function(self, key)
	-- Check that the options table exists.
	-- This is just to silence an error message.
	local rows1 = self.__db:query([[SELECT name FROM sqlite_master WHERE type='table' AND name='options';]])
	if not rows1 then return end
	if #rows1 == 0 then return end
	-- Get the value from the options table
	local rows = self.__db:query([[SELECT value FROM options WHERE key=?;]], {key})
	if not rows then return end
	for k,v in ipairs(rows) do
		return v[1]
	end
end

--- Sets an account database option.
-- @param self AccountDatabase.
-- @param key Option name.
-- @param value Option value.
AccountDatabase.set_option = function(self, key, value)
	self.__db:query([[REPLACE INTO options (key,value) VALUES (?,?);]], {key, value})
end

return AccountDatabase

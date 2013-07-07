--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.account_database
-- @alias AccountDatabase

local Account = require("core/server/account")
local Class = require("system/class") 
local Password = require("system/password")
local Serializer = require("system/serializer")

--- TODO:doc
-- @type AccountDatabase
local AccountDatabase = Class("AccountDatabase")
AccountDatabase.account_version = "2"
AccountDatabase.serializer = Serializer{}

--- Creates a new account database.
-- @param clss AccountDatabase class.
-- @param db Database.
-- @return AccountDatabase.
AccountDatabase.new = function(clss, db)
	local self = Class.new(clss)
	self.db = db
	local salt = self:get_account_option("password_salt")
	local version = self:get_account_option("account_version")
	if salt and version == self.account_version then
		self.password_salt = salt
	else
		self:reset()
	end
	return self
end

--- Resets the account database.
-- @param self AccountDatabase.
AccountDatabase.reset = function(self)
	-- Initialize tables.
	self.db:query([[DROP TABLE IF EXISTS accounts;]])
	self.db:query([[DROP TABLE IF EXISTS account_fields;]])
	self.db:query([[DROP TABLE IF EXISTS options;]])
	self.db:query(
		[[CREATE TABLE accounts (
		login TEXT PRIMARY KEY,
		password TEXT,
		permissions INTEGER,
		character TEXT,
		spawn_point TEXT);]])
	self.db:query([[CREATE TABLE account_fields (
		account TEXT,
		name TEXT,
		value TEXT,
		PRIMARY KEY(account,name));]])
	self.db:query(
		[[CREATE TABLE options (
		key TEXT PRIMARY KEY,
		value TEXT);]])
	-- Initialize password hashing.
	self.password_salt = Password:random_salt()
	self:set_account_option("password_salt", self.password_salt)
	-- Set the version number.
	self:set_account_option("account_version", self.account_version)
end

--- Creates an empty account.
-- @param self AccountDatabase.
-- @param login Login name.
-- @param password Password.
-- @return Account.
AccountDatabase.create_account = function(self, login, password)
	local hash = self:hash_password(login, password)
	local account = Account(login, hash)
	self:save_account(account)
	return account
end

--- Creates a hashed version of the password.
-- @param self AccountDatabase.
-- @param login Login name.
-- @param password Password string.
-- @return Hashed password string.
AccountDatabase.hash_password = function(self, login, password)
	return Password:hash(password, self.password_salt)
end

--- Loads an account from the account database.
-- @param self AccountDatabase.
-- @param login Login name.
-- @param password Password.
-- @return Account or nil, status message or nil.
AccountDatabase.load_account = function(self, login, password)
	-- Load the account data.
	local r = self.db:query(
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
	local rows = self.db:query(
		[[SELECT name,value FROM account_fields WHERE account=?]], {login})
	self.serializer:read(account, rows)
	-- Return the account and the status message.
	return account, "login successful"
end

--- Saves a player account.
-- @param self AccountDatabase.
-- @param account Account.
-- @param object Player object, or nil.
AccountDatabase.save_account = function(self, account, object)
	-- Save the account.
	self.db:query(
		[[REPLACE INTO accounts
		(login,password,permissions,character,spawn_point)
		VALUES (?,?,?,?,?);]],
		{account.login, account.password, account.permissions, object and object:get_id(), account.spawn_point and tostring(account.spawn_point)})
	-- Save the fields.
	self.db:query([[DELETE FROM account_fields WHERE account=?;]], {account.login})
	self.serializer:write(account, function(name, value)
		self.db:query(
			[[REPLACE INTO account_fields
			(account,name,value)
			VALUES (?,?,?);]],
			{account.login, name, value})
	end)
end

--- Saves all active player accounts.
-- @param self AccountDatabase.
-- @param erase True to erase characters, "all" to erase all account data.
AccountDatabase.save_accounts = function(self, erase)
	-- Delete accounts or characters.
	if erase == "all" then
		self.db:query([[DELETE FROM accounts;]])
	elseif erase then
		self.db:query([[UPDATE accounts SET character = NULL;]])
		self.db:query([[UPDATE accounts SET spawn_point = NULL;]])
	end
	-- Write accounts.
	for k,v in pairs(Server.players_by_client) do
		self:save_account(v.account, v)
	end
end

--- Gets an account database option.
-- @param self AccountDatabase.
-- @param key Option name.
-- @return Option value.
AccountDatabase.get_account_option = function(self, key)
	-- Check that the options table exists.
	-- This is just to silence an error message.
	local rows1 = self.db:query([[SELECT name FROM sqlite_master WHERE type='table' AND name='options';]])
	if not rows1 then return end
	if #rows1 == 0 then return end
	-- Get the value from the options table
	local rows = self.db:query([[SELECT value FROM options WHERE key=?;]], {key})
	if not rows then return end
	for k,v in ipairs(rows) do
		return v[1]
	end
end

--- Sets an account database option.
-- @param self AccountDatabase.
-- @param key Option name.
-- @param value Option value.
AccountDatabase.set_account_option = function(self, key, value)
	self.db:query(
		[[REPLACE INTO options (key,value) VALUES (?,?);]], {key, value})
end

return AccountDatabase

require "system/class"
require "system/password"

AccountDatabase = Class()
AccountDatabase.account_version = "1"

--- Creates a new account database.
-- @param clss AccountDatabase class.
-- @param db Database.
-- @returns AccountDatabase.
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
	self.db:query([[DROP TABLE IF EXISTS options;]])
	self.db:query(
		[[CREATE TABLE accounts (
		login TEXT PRIMARY KEY,
		password TEXT,
		permissions INTEGER,
		character TEXT,
		spawn_point TEXT);]])
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

--- Loads an account from the account database.
-- @param self AccountDatabase.
-- @param login Login name.
-- @return Account database row, or nil.
AccountDatabase.load_account = function(self, login)
	local r = self.db:query(
		[[SELECT login,password,permissions,character,spawn_point
		FROM accounts WHERE login=?;]], {login})
	for k,v in ipairs(r) do
		return v
	end
end

--- Saves a player account.
-- @param self AccountDatabase.
-- @param account Account.
-- @param object Player object, or nil.
AccountDatabase.save_account = function(self, account, object)
	self.db:query(
		[[REPLACE INTO accounts
		(login,password,permissions,character,spawn_point)
		VALUES (?,?,?,?,?);]],
		{account.login, account.password, account.permissions, object and object.id, account.spawn_point and tostring(account.spawn_point)})
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

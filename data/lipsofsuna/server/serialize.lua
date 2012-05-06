require "common/unlocks"

Serialize = Class()
Serialize.game_version = "4"
Serialize.account_version = "1"
Serialize.object_version = "2"

--- Initializes the serializer.
-- @param clss Serialize class.
Serialize.init = function(clss)
	-- Create the save database.
	clss.db = Database{name = "save" .. Settings.file .. ".sqlite"}
	clss:init_game_database()
	clss:init_object_database()
	clss.sectors = Sectors{database = clss.db}
	Sectors.instance = clss.sectors
	-- Create the account database.
	clss.accounts = Database{name = "accounts" .. Settings.file .. ".sqlite"}
	clss:init_account_database(self)
end

--- Loads everything except map data.
-- @param clss Serialize class.
Serialize.load = function(clss)
	clss:load_generator()
	clss:load_markers()
	clss:load_quests()
	clss:load_static_objects()
end

--- Loads map generator data from the database.
-- @param clss Serialize class.
Serialize.load_generator = function(clss)
	-- Load settings.
	local r1 = clss.db:query("SELECT key,value FROM generator_settings;")
	local f1 = {
		seed1 = function(v) Generator.inst.seed1 = tonumber(v) end,
		seed2 = function(v) Generator.inst.seed2 = tonumber(v) end,
		seed3 = function(v) Generator.inst.seed3 = tonumber(v) end}
	for k,v in ipairs(r1) do
		local f = f1[v[1]]
		if f then f(v[2]) end
	end
	-- Load special sectors.
	local r2 = clss.db:query("SELECT id,value FROM generator_sectors;")
	for k,v in ipairs(r2) do
		Generator.inst.sectors[v[1]] = v[2]
	end
end

--- Loads map markers from the database.
-- @param clss Serialize class.
Serialize.load_markers = function(clss)
	local r = clss.db:query("SELECT name,id,x,y,z,unlocked,discoverable FROM markers;")
	for k,v in ipairs(r) do
		Marker{name = v[1], target = v[2], position = Vector(v[3], v[4], v[5]),
			unlocked = (v[6] == 1), discoverable = (v[7] == 1)}
	end
end

--- Loads quests from the database.
-- @param clss Serialize class.
Serialize.load_quests = function(clss)
	local r = clss.db:query("SELECT name,status,desc,marker FROM quests;")
	for k,v in ipairs(r) do
		local quest = Quest:find{name = v[1]}
		if quest then
			quest:update{status = v[2], text = v[3], marker = v[4]}
		end
	end
	local r = clss.db:query("SELECT name,value FROM dialog_flags;")
	for k,v in ipairs(r) do
		Dialog.flags[v[1]] = v[2]
	end
end

Serialize.erase_world = function(self)
	self.database:query([[DELETE FROM objects_data;]])
end

--- Saves everything.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save = function(clss, erase)
	if erase then clss:clear_objects() end
	clss.sectors:save_world(erase)
	clss:save_generator(erase)
	clss:save_markers(erase)
	clss:save_quests(erase)
	clss:save_accounts(erase)
	Unlocks:write_db()
end

--- Saves the map generator state.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save_generator = function(clss, erase)
	clss.db:query("BEGIN TRANSACTION;")
	if erase then
		clss.db:query("DELETE FROM generator_settings;")
		clss.db:query("DELETE FROM generator_sectors;")
	end
	clss.db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed1", tostring(Generator.inst.seed1)})
	clss.db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed2", tostring(Generator.inst.seed2)})
	clss.db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed3", tostring(Generator.inst.seed3)})
	for k,v in pairs(Generator.inst.sectors) do
		clss.db:query("REPLACE INTO generator_sectors (id,value) VALUES (?,?);", {k, v})
	end
	clss.db:query("END TRANSACTION;")
end

--- Saves a map marker.
-- @param clss Serialize class.
-- @param marker Map marker.
Serialize.save_marker = function(clss, marker)
	clss.db:query("BEGIN TRANSACTION;")
	clss.db:query("REPLACE INTO markers (name,id,x,y,z,unlocked,discoverable) VALUES (?,?,?,?,?,?,?);",
		{marker.name, marker.target, marker.position.x, marker.position.y, marker.position.z,
		 marker.unlocked and 1 or 0, marker.discoverable and 1 or 0})
	clss.db:query("END TRANSACTION;")
end

--- Saves all map markers.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save_markers = function(clss, erase)
	clss.db:query("BEGIN TRANSACTION;")
	if erase then
		clss.db:query("DELETE FROM markers;")
	end
	for k,v in pairs(Marker.dict_name) do
		clss.db:query("REPLACE INTO markers (name,id,x,y,z,unlocked,discoverable) VALUES (?,?,?,?,?,?,?);",
			{k, v.target, v.position.x, v.position.y, v.position.z,
			 v.unlocked and 1 or 0, v.discoverable and 1 or 0})
	end
	clss.db:query("END TRANSACTION;")
end

--- Saves a quest.
-- @param clss Serialize class.
-- @param quest Quest.
Serialize.save_quest = function(clss, quest)
	clss.db:query("BEGIN TRANSACTION;")
	clss.db:query("REPLACE INTO quests (name,status,desc,marker) VALUES (?,?,?,?);",
		{quest.name, quest.status, quest.text, quest.marker})
	clss.db:query("END TRANSACTION;")
end

--- Saves all quests.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save_quests = function(clss, erase)
	clss.db:query("BEGIN TRANSACTION;")
	clss.db:query("DELETE FROM quests;")
	clss.db:query("DELETE FROM dialog_flags;")
	for k,v in pairs(Quest.dict_name) do
		clss.db:query("REPLACE INTO quests (name,status,desc,marker) VALUES (?,?,?,?);",
			{k, v.status, v.text, v.marker})
	end
	for k,v in pairs(Dialog.flags) do
		clss.db:query("REPLACE INTO dialog_flags (name,value) VALUES (?,?);", {k, v})
	end
	clss.db:query("END TRANSACTION;")
end

------------------------------------------------------------------------------
-- Game database.

--- Initializes the game database.
-- @param self Serialize class.
-- @param reset True to clear the database.
Serialize.init_game_database = function(self, reset)
	-- Check if changes are needed.
	local version = self:get_value("game_version")
	if not reset and version == self.game_version then return end
	-- Initialize tables.
	self.db:query([[DROP TABLE IF EXISTS dialog_flags;]])
	self.db:query([[DROP TABLE IF EXISTS dialog_variables;]])
	self.db:query([[DROP TABLE IF EXISTS generator_sectors;]])
	self.db:query([[DROP TABLE IF EXISTS generator_settings;]])
	self.db:query([[DROP TABLE IF EXISTS markers;]])
	self.db:query([[DROP TABLE IF EXISTS options;]])
	self.db:query([[DROP TABLE IF EXISTS quests;]])
	self.db:query([[DROP TABLE IF EXISTS unlocks;]])
	self.db:query(
		[[CREATE TABLE options (
		key TEXT PRIMARY KEY,
		value TEXT);]])
	self.db:query([[CREATE TABLE dialog_flags (
		name TEXT PRIMARY KEY,
		value TEXT);]])
	self.db:query([[CREATE TABLE dialog_variables (
		id INTEGER,
		key TEXT,
		value TEXT,
		PRIMARY KEY(id,key));]])
	self.db:query([[CREATE TABLE generator_sectors (
		id INTEGER PRIMARY KEY,
		value TEXT);]])
	self.db:query([[CREATE TABLE generator_settings (
		key TEXT PRIMARY KEY,
		value TEXT);]])
	self.db:query([[CREATE TABLE markers (
		name TEXT PRIMARY KEY,
		id INTEGER,
		x FLOAT,
		y FLOAT,
		z FLOAT,
		unlocked INTENGER,
		discoverable INTEGER);]])
	self.db:query([[CREATE TABLE quests (
		name TEXT PRIMARY KEY,
		status TEXT,
		desc TEXT,
		marker TEXT);]])
	self.db:query([[CREATE TABLE unlocks (
		type TEXT,
		name TEXT,
		PRIMARY KEY(type,name));]])
	-- Set the version number.
	self:set_value("game_version", self.game_version)
end

--- Saves an unlock to the database.
-- @param self Serialize class.
-- @param type Type of the unlocked item.
-- @param name Name of the unlocked item.
-- @param value True to unlock, false to lock.
Serialize.save_unlock = function(self, type, name, value)
	if value then
		self.db:query([[REPLACE INTO unlocks (type,name) VALUES (?,?);]], {type, name})
	else
		self.db:query([[DELETE FROM unlocks WHERE type=? AND name=?;]], {type, name})
	end
end

--- Gets a dialog variable by the owner object and variable name.
-- @param self Serialize class.
-- @param object Object.
-- @param name Variable name.
-- @return Variable value, or nil.
Serialize.get_dialog_variable = function(self, object, name)
	local rows = self.db:query([[SELECT value FROM dialog_variables WHERE id=? AND key=?;]], {object.id, name})
	if not rows then return end
	for k,v in ipairs(rows) do
		return v[1]
	end
end

--- Sets the given dialog variable of the owner object.
-- @param self Serialize class.
-- @param object Object.
-- @param name Variable name.
-- @param value Variable value.
Serialize.set_dialog_variable = function(self, object, name, value)
	if value then
		self.db:query([[REPLACE INTO dialog_variables (id,key,value) VALUES (?,?,?);]], {object.id, name, value})
	else
		self.db:query([[DELETE FROM dialog_variables WHERE id=? AND key=?;]], {object.id, name})
	end
end

--- Gets all dialog variables for the object.
-- @param self Serialize class.
-- @param object Object.
-- @return Dictionary of variables.
Serialize.get_dialog_variables = function(self, object)
	local res = {}
	local rows = self.db:query([[SELECT key,value FROM dialog_variables WHERE id=?;]], {object.id})
	if rows then
		for k,v in ipairs(rows) do
			res[v[1]] = v[2]
		end
	end
	return res
end

--- Gets a value from the key-value database.
-- @param self Serialize class.
-- @param key Key string.
-- @return Value string or nil.
Serialize.get_value = function(self, key)
	local rows = self.db:query([[SELECT value FROM options WHERE key=?;]], {key})
	if not rows then return end
	for k,v in ipairs(rows) do
		return v[1]
	end
end

--- Stores a value to the key-value database.
-- @param self Serialize class.
-- @param key Key string.
-- @param value Value string.
Serialize.set_value = function(self, key, value)
	self.db:query([[REPLACE INTO options (key,value) VALUES (?,?);]], {key, value})
end

------------------------------------------------------------------------------
-- Account database.

--- Initializes the object database.
-- @param self Serialize class.
-- @param reset True to clear the database.
Serialize.init_account_database = function(self, reset)
	-- Check if changes are needed.
	local salt = self:get_account_option("password_salt")
	local version = self:get_account_option("account_version")
	if not reset and salt and version == self.account_version then
		self.accounts.password_salt = self
		return
	end
	-- Initialize tables.
	self.accounts:query([[DROP TABLE IF EXISTS accounts;]])
	self.accounts:query([[DROP TABLE IF EXISTS options;]])
	self.accounts:query(
		[[CREATE TABLE accounts (
		login TEXT PRIMARY KEY,
		password TEXT,
		permissions INTEGER,
		character TEXT,
		spawn_point TEXT);]])
	self.accounts:query(
		[[CREATE TABLE options (
		key TEXT PRIMARY KEY,
		value TEXT);]])
	-- Initialize password hashing.
	self.accounts.password_salt = Password:random_salt()
	self:set_account_option("password_salt", self.accounts.password_salt)
	-- Set the version number.
	self:set_account_option("account_version", self.account_version)
end

--- Loads an account from the account database.
-- @param self Serialize class.
-- @param login Login name.
-- @return Account database row, or nil.
Serialize.load_account = function(self, login)
	local r = self.accounts:query(
		[[SELECT login,password,permissions,character,spawn_point
		FROM accounts WHERE login=?;]], {login})
	for k,v in ipairs(r) do
		return v
	end
end

--- Saves a player account.
-- @param self Serialize class.
-- @param account Account.
-- @param object Player object, or nil.
Serialize.save_account = function(self, account, object)
	self.accounts:query(
		[[REPLACE INTO accounts
		(login,password,permissions,character,spawn_point)
		VALUES (?,?,?,?,?);]],
		{account.login, account.password, account.permissions, object and object.id, account.spawn_point and tostring(account.spawn_point)})
end

--- Saves all active player accounts.
-- @param self Serialize class.
-- @param erase True to erase characters, "all" to erase all account data.
Serialize.save_accounts = function(self, erase)
	-- Delete accounts or characters.
	if erase == "all" then
		self.accounts:query([[DELETE FROM accounts;]])
	elseif erase then
		self.accounts:query([[UPDATE accounts SET character = NULL;]])
		self.accounts:query([[UPDATE accounts SET spawn_point = NULL;]])
	end
	-- Write accounts.
	for k,v in pairs(Player.clients) do
		self:save_account(v.account, v)
	end
end

--- Gets an account database option.
-- @param self Serialize class.
-- @param key Option name.
-- @return Option value.
Serialize.get_account_option = function(self, key)
	local rows = self.accounts:query(
		[[SELECT value FROM options WHERE key=?;]], {key})
	if not rows then return end
	for k,v in ipairs(rows) do
		return v[1]
	end
end

--- Sets an account database option.
-- @param self Serialize class.
-- @param key Option name.
-- @param value Option value.
Serialize.set_account_option = function(self, key, value)
	self.accounts:query(
		[[REPLACE INTO options (key,value) VALUES (?,?);]], {key, value})
end

------------------------------------------------------------------------------
-- Object database.

--- Initializes the object database.
-- @param self Serialize class.
-- @param reset True to clear the database.
Serialize.init_object_database = function(self, reset)
	-- Check if changes are needed.
	local version = self:get_value("object_version")
	if not reset and version == self.object_version then return end
	-- Initialize tables.
	self.db:query([[DROP TABLE IF EXISTS object_data;]])
	self.db:query([[DROP TABLE IF EXISTS object_inventory;]])
	self.db:query([[DROP TABLE IF EXISTS object_sectors;]])
	self.db:query([[DROP TABLE IF EXISTS object_skills;]])
	self.db:query([[DROP TABLE IF EXISTS object_stats;]])
	self.db:query([[CREATE TABLE object_data (
		id INTEGER PRIMARY KEY,
		type TEXT,
		data TEXT);]])
	self.db:query([[CREATE TABLE object_inventory (
		id INTEGER PRIMARY KEY,
		parent INTEGER,
		offset INTEGER,
		slot TEXT);]])
	self.db:query([[CREATE TABLE object_sectors (
		id INTEGER PRIMARY KEY,
		sector INTEGER);]])
	self.db:query([[CREATE TABLE object_skills (
		id INTEGER,
		name TEXT,
		PRIMARY KEY(id,name));]])
	self.db:query([[CREATE TABLE object_stats (
		id INTEGER,
		name TEXT,
		value FLOAT,
		PRIMARY KEY(id,name));]])
	-- Set the version number.
	self:set_value("object_version", self.object_version)
end

--- Removes all objects from the database.
-- @param self Serialize class.
Serialize.clear_objects = function(self)
	self.db:query([[DELETE FROM object_data;]])
	self.db:query([[DELETE FROM object_inventory;]])
	self.db:query([[DELETE FROM object_sectors;]])
	self.db:query([[DELETE FROM object_skills;]])
	self.db:query([[DELETE FROM object_stats;]])
end

--- Reads an object from the database.
-- @param self Serialize class.
-- @param id Object ID.
-- @param type Object type.
-- @param data Object data.
-- @return Object.
Serialize.load_object = function(self, id, type, data)
	-- Load the data string.
	if not data then return end
	local func = loadstring(data)
	if not func then return end
	-- Execute the load function.
	local ok,ret = pcall(func)
	if not ok then
		print(ret)
		return
	end
	if not ret then return end
	-- Read additional data.
	ret:read_db(self.db)
	return ret
end

--- Reads all objects in a sector.
-- @param self Serialize class.
-- @param parent Object.
-- @return List of objects.
Serialize.load_object_inventory = function(self, parent)
	local objects = {}
	local rows = self.db:query(
		[[SELECT b.id,b.type,b.data,a.offset,a.slot FROM
		object_inventory AS a INNER JOIN
		object_data AS b WHERE
		a.parent=? AND a.id=b.id]], {parent.id})
	for k,v in ipairs(rows) do
		local obj = Serialize:load_object(v[1], v[2], v[3])
		if obj then
			parent.inventory:set_object(v[4], obj)
			if v[5] then
				parent.inventory:equip_index(v[4], v[5])
			end
			table.insert(objects, obj)
		end
	end
	return objects
end

--- Reads the skills of the object from the database.
-- @param self Serialize class.
-- @param parent Object.
Serialize.load_object_skills = function(self, parent)
	local rows = self.db:query(
		[[SELECT name FROM object_skills WHERE id=?]], {parent.id})
	for k,v in ipairs(rows) do
		parent.skills:add_without_requirements(v[1])
	end
	parent.skills:remove_invalid()
end

--- Reads the skills of the object from the database.
-- @param self Serialize class.
-- @param parent Object.
Serialize.load_object_stats = function(self, parent)
	local rows = self.db:query(
		[[SELECT name,value FROM object_stats WHERE id=?]], {parent.id})
	for k,v in ipairs(rows) do
		parent.stats:set_value(v[1], v[2])
	end
end

--- Reads a player object from the database.
-- @param self Serialize class.
-- @param account Account.
-- @return Object, or nil.
Serialize.load_player_object = function(self, account)
	if not account.character then return end
	local rows = self.db:query(
		[[SELECT id,type,data FROM object_data
		WHERE type=? AND id=?]],
		{"player", account.character})
	for k,v in ipairs(rows) do
		return self:load_object(v[1], v[2], v[3])
	end
end

--- Reads all objects in a sector.
-- @param self Serialize class.
-- @param sector Sector index.
-- @return List of objects.
Serialize.load_sector_objects = function(self, sector)
	local objects = {}
	local rows = self.db:query(
		[[SELECT b.id,b.type,b.data FROM
		object_sectors AS a INNER JOIN
		object_data AS b WHERE
		a.sector=? AND a.id=b.id]], {sector})
	for k,v in ipairs(rows) do
		local obj = Serialize:load_object(v[1], v[2], v[3])
		if obj then
			obj.realized = true
			table.insert(objects, obj)
		end
	end
	return objects
end

--- Reads all static objects.
-- @param self Serialize class.
-- @return List of objects.
Serialize.load_static_objects = function(self)
	local objects = {}
	local rows = self.db:query(
		[[SELECT id,type,data FROM object_data WHERE type=?]], {"static"})
	for k,v in ipairs(rows) do
		local obj = self:load_object(v[1], v[2], v[3])
		if obj then
			obj.realized = true
			table.insert(objects, obj)
		end
	end
	return objects
end

--- Writes an object to the database.
-- @param self Serialize class.
-- @param object Object.
Serialize.save_object = function(self, object)
	object:write_db(self.db)
end

--- Writes objects in the given sector to the database.
-- @param self Serialize class.
-- @param sector Sector number.
Serialize.save_sector_objects = function(self, sector)
	self.db:query([[DELETE FROM object_sectors WHERE sector=?;]], {sector})
	local objs = Object:find{sector = sector}
	for k,v in pairs(objs) do
		v:write_db(self.db)
	end
end

--- Writes all static objects to the database.
-- @param self Serialize class.
Serialize.save_static_objects = function(self)
	for k,v in pairs(Staticobject.dict_id) do
		if v.realized then
			v:write_db(self.db)
		end
	end
end

------------------------------------------------------------------------------

Serialize:init()

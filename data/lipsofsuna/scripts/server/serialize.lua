Serialize = Class()
Serialize.data_version = "4"

--- Initializes the serializer.
-- @param clss Serialize class.
Serialize.init = function(clss)
	clss.accounts = Database{name = "accounts" .. Settings.file .. ".sqlite"}
	clss.accounts:query("CREATE TABLE IF NOT EXISTS accounts (login TEXT PRIMARY KEY,password TEXT,permissions INTEGER,character TEXT);")
	clss.db = Database{name = "save" .. Settings.file .. ".sqlite"}
	clss.sectors = Sectors{database = clss.db}
	clss.db:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT PRIMARY KEY,value TEXT);")
	if clss:get_value("data_version") ~= clss.data_version then
		clss.db:query("DROP TABLE IF EXISTS markers;")
		clss.db:query("DROP TABLE IF EXISTS quests;")
	end
	clss.db:query("CREATE TABLE IF NOT EXISTS markers (name TEXT PRIMARY KEY,id INTEGER,x FLOAT,y FLOAT,z FLOAT);")
	clss.db:query("CREATE TABLE IF NOT EXISTS quests (name TEXT PRIMARY KEY,progress FLOAT,status TEXT,desc TEXT,marker TEXT);")
	Sectors.instance = clss.sectors
end

--- Makes a string out of an inventory and saves the items to the database.
-- @param clss Serialize class.
-- @param inv Inventory.
-- @return String.
Serialize.encode_inventory = function(clss, inv)
	if not inv then return "" end
	local str = ""
	for slot,obj in pairs(inv.slots) do
		obj:save()
		str = string.format("%sself.inventory:set_object{slot=%s,object=Object:load{id=%s}}\n",
			str, serialize(slot), serialize(obj.id))
	end
	return str
end

--- Makes a string out of skills.
-- @param clss Serialize class.
-- @param skills Skills.
-- @return String.
Serialize.encode_skills = function(clss, skills)
	if not skills then return "" end
	local str = string.format("self.skills.enabled=%s\n", serialize(skills.enabled))
	for k,v in pairs(skills:get_names()) do
		local val = skills:get_value{skill = v}
		local max = skills:get_maximum{skill = v}
		str = string.format("%sself.skills:set{skill=%s,maximum=%s,value=%s}\n",
			str, serialize(v), serialize(max), serialize(val))
	end
	return str
end

--- Gets a value from the key-value database.
-- @param clss Serialize class.
-- @param key Key string.
-- @return Value string or nil.
Serialize.get_value = function(clss, key)
	local rows = clss.db:query("SELECT value FROM keyval WHERE key=?;", {key})
	for k,v in ipairs(rows) do
		return v[1]
	end
end

--- Loads everything except map data.
-- @param clss Serialize class.
Serialize.load = function(clss)
	clss:load_markers()
	clss:load_quests()
end

--- Loads an account from the account database.
-- @param clss Serialize class.
-- @param login Login name.
-- @return Account database row or nil.
Serialize.load_account = function(clss, login)
	local r = clss.accounts:query("SELECT login,password,permissions,character FROM accounts WHERE login=?;", {login})
	for k,v in ipairs(r) do
		return v
	end
end

--- Loads map markers from the database.
-- @param clss Serialize class.
Serialize.load_markers = function(clss)
	local r = clss.db:query("SELECT name,id,x,y,z FROM markers;")
	for k,v in ipairs(r) do
		Marker{name = v[1], target = v[2], position = Vector(v[3], v[4], v[5])}
	end
end

--- Loads quests from the database.
-- @param clss Serialize class.
Serialize.load_quests = function(clss)
	local r = clss.db:query("SELECT name,progress,status,desc,marker FROM quests;")
	for k,v in ipairs(r) do
		local quest = Quest:find{name = v[1]}
		if quest then
			quest:update{progress = v[2], status = v[3], text = v[4], marker = v[5]}
		end
	end
end

--- Saves everything.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save = function(clss, erase)
	clss.sectors:save_world(erase)
	clss:save_markers(erase)
	clss:save_quests(erase)
	clss:save_accounts(erase)
end

--- Saves a player account.
-- @param clss Serialize class.
-- @param account Account.
-- @param object Player object or nil.
Serialize.save_account = function(clss, account, object)
	clss.accounts:query("BEGIN TRANSACTION;")
	clss.accounts:query("REPLACE INTO accounts (login,password,permissions,character) VALUES (?,?,?,?);",
		{account.login, account.password, account.permissions, object and object:write()})
	clss.accounts:query("END TRANSACTION;")
end

--- Saves all active player accounts.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save_accounts = function(clss, erase)
	if erase then
		clss.accounts:query("DELETE FROM accounts;")
	end
	for k,v in pairs(Player.clients) do
		clss:save_account(v.account, v)
	end
end

--- Saves a map marker.
-- @param clss Serialize class.
-- @param marker Map marker.
Serialize.save_marker = function(clss, marker)
	clss.db:query("BEGIN TRANSACTION;")
	clss.db:query("REPLACE INTO markers (name,id,x,y,z) VALUES (?,?,?,?,?);",
		{marker.name, marker.target, marker.position.x, marker.position.y, marker.position.z})
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
		clss.db:query("REPLACE INTO markers (name,id,x,y,z) VALUES (?,?,?,?,?);",
			{k, v.target, v.position.x, v.position.y, v.position.z})
	end
	clss.db:query("END TRANSACTION;")
end

--- Saves a quest.
-- @param clss Serialize class.
-- @param quest Quest.
Serialize.save_quest = function(clss, quest)
	clss.db:query("BEGIN TRANSACTION;")
	clss.db:query("REPLACE INTO quests (name,progress,status,desc,marker) VALUES (?,?,?,?,?);",
		{quest.name, quest.progress, quest.status, quest.text, quest.marker})
	clss.db:query("END TRANSACTION;")
end

--- Saves all quests.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save_quests = function(clss, erase)
	clss.db:query("BEGIN TRANSACTION;")
	if erase then
		clss.db:query("DELETE FROM quests;")
	end
	for k,v in pairs(Quest.dict_name) do
		clss.db:query("REPLACE INTO quests (name,progress,status,desc) VALUES (?,?,?,?);",
			{k, v.progress, v.status, v.text, v.marker})
	end
	clss.db:query("END TRANSACTION;")
end

--- Stores a value to the key-value database.
-- @param clss Serialize class.
-- @param key Key string.
-- @param value Value string.
Serialize.set_value = function(clss, key, value)
	clss.db:query("REPLACE INTO keyval (key,value) VALUES (?,?);", {key, value})
end

Serialize:init()

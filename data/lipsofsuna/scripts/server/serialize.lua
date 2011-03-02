Serialize = Class()
Serialize.data_version = "4"

--- Initializes the serializer.
-- @param clss Serialize class.
Serialize.init = function(clss)
	clss.db = Database{name = "save" .. Settings.file .. ".sqlite"}
	clss.sectors = Sectors{database = clss.db}
	clss.db:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT PRIMARY KEY,value TEXT);")
	if clss:get_value("data_version") ~= clss.data_version then
		clss.db:query("DROP TABLE markers;")
		clss.db:query("DROP TABLE quests;")
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
		str = str .. "self.inventory:set_object{slot=" .. serialize_value(slot) ..
			",object=Object:load{id=" .. serialize_value(obj.id) ..  "}}\n"
	end
	return str
end

--- Makes a string out of skills.
-- @param clss Serialize class.
-- @param skills Skills.
-- @return String.
Serialize.encode_skills = function(clss, skills)
	if not skills then return "" end
	local str = "self.skills.enabled=" .. serialize_value(skills.enabled) .. "\n"
	for k,v in pairs(skills:get_names()) do
		local val = skills:get_value{skill = v}
		str = str .. "self.skills:set_value{skill=" .. serialize_value(v) ..
			",value=" .. serialize_value(val) .. "}\n"
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

-----------

deserialize_table = function(str)
	return assert(loadstring("return " .. str))()
end

serialize_value = function(val)
	if type(val) == "string" then
		return string.format("%q", val)
	elseif type(val) == "table" then
		return serialize_table(val)
	elseif type(val) == "number" or type(val) == "boolean" then
		return tostring(val)
	elseif type(val) == "userdata" then
		return tostring(val)
	end
	return "nil"
end

serialize_table = function(tbl)
	local str = "{"
	for k,v in pairs(tbl) do
		local ks = serialize_value(k)
		local vs = serialize_value(v)
		if ks and vs then
			if string.len(str) > 1 then str = str .. "," end
			str = str .. "[" .. ks .. "]=" .. vs
		end
	end
	return str .. "}"
end

serialize_npc = function(npc)
	local str = "local npc=Npc{owner=self" ..
		",alert=" .. serialize_value(npc.alert) ..
		",radius=" .. serialize_value(npc.radius) ..
		",refresh=" .. serialize_value(npc.refresh) .. "}\n"
	return str
end

serialize_quat = function(quat)
	local str = "Quaternion(" ..
		serialize_value(quat.x) .. "," ..
		serialize_value(quat.y) .. "," ..
		serialize_value(quat.z) .. "," ..
		serialize_value(quat.w) .. ")"
	return str
end

serialize_vector = function(vector)
	local str = "Vector(" ..
		serialize_value(vector.x) .. "," ..
		serialize_value(vector.y) .. "," ..
		serialize_value(vector.z) .. ")"
	return str
end

Serialize = Class()

--- Initializes the serializer.
-- @param clss Serialize class.
Serialize.init = function(clss)
	clss.db = Database{name = "save.db"}
	clss.sectors = Sectors{database = clss.db}
	clss.db:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT,value TEXT);")
	Sectors.instance = clss.sectors
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

serialize_inventory = function(inv)
	local str = "local inv = Inventory{owner=self, size=" .. serialize_value(inv.size) .. "}\n"
	for i=1,inv.size do
		local obj = inv:get_object{slot = i}
		if obj then
			obj:write()
			str = str .. "inv:set_object{slot=" .. serialize_value(i) ..
				",object=Object:new{id=" .. serialize_value(obj.id) ..  "}}\n"
		end
	end
	return str
end

serialize_npc = function(npc)
	local str = "local npc=Npc{owner=self" ..
		",alert=" .. serialize_value(npc.alert) ..
		",radius=" .. serialize_value(npc.radius) ..
		",refresh=" .. serialize_value(npc.refresh) .. "}\n"
	return str
end

serialize_skills = function(skills)
	if not skills then return "" end
	local str = "self.skills.enabled=" .. serialize_value(skills.enabled) .. "\n"
	for k,v in pairs(skills:get_names()) do
		local max = skills:get_maximum{skill = v}
		local val = skills:get_value{skill = v}
		local regn = skills:get_regen{skill = v}
		local prot = skills:get_protect{skill = v}
		str = str .. "self.skills:register{prot=" .. serialize_value(prot) ..
			",skill=" .. serialize_value(v) ..
			",value=" .. serialize_value(val) ..
			",maximum=" .. serialize_value(max) ..
			",regen=" .. serialize_value(regn) .. "}\n"
	end
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

Serialize = Class()

--- Initializes the serializer.
-- @param clss Serialize class.
Serialize.init = function(clss)
	clss.db = Database{name = "save.db"}
	clss.db:query("CREATE TABLE IF NOT EXISTS objects (id INTEGER PRIMARY KEY,sector UNSIGNED INTEGER,data TEXT);");
	clss.db:query("CREATE TABLE IF NOT EXISTS terrain (sector INTEGER PRIMARY KEY,data BLOB);");
end

--- Reads a sector from the database.
-- @param class Serialize class.
-- @param sector Sector index.
Serialize.load_sector = function(clss, sector)
	-- Load terrain.
	local rows = clss.db:query("SELECT * FROM terrain WHERE sector=?;", {sector})
	for k,v in ipairs(rows) do
		Voxel:paste_region{sector = sector, packet = v[2]}
	end
	-- Load objects.
	local rows = clss.db:query("SELECT * FROM objects WHERE sector=?;", {sector})
	for k,v in ipairs(rows) do
		local func = assert(loadstring("return function()\n" .. v[3] .. "\nend"))()
		if func then
			local object = func()
			if object then object.realized = true end
		end
	end
end

--- Saves a sector to the database.
-- @param class Serialize class.
-- @param sector Sector index.
Serialize.save_sector = function(clss, sector)
	-- Erase old data.
	clss.db:query("DELETE FROM objects WHERE sector=?;", {sector})
	clss.db:query("DELETE FROM terrain WHERE sector=?;", {sector})
	-- Write objects.
	local objs = Object:find{sector = sector}
	for k,v in pairs(objs) do
		local data = v:write()
		if data and not Class:check{data = v, name = "Player"} then
			clss.db:query("INSERT INTO objects (id,sector,data) VALUES (?,?,?);", {v.id, sector, data})
		end
	end
	-- Write terrain.
	local data = Voxel:copy_region{sector = sector}
	clss.db:query("INSERT INTO terrain (sector,data) VALUES (?,?);", {sector, data})
end

--- Saves all active sectors to the database.
-- @param class Serialize class.
Serialize.save_world = function(clss)
	clss.db:query("BEGIN TRANSACTION;")
	for k,v in pairs(Program.sectors) do
		clss:save_sector(k)
	end
	clss.db:query("END TRANSACTION;")
end

Serialize:init()
Eventhandler{type = "sector-load", func = function(self, args)
	Serialize:load_sector(args.sector)
end}
Timer{delay = 2, func = function(self, args)
	local written = 0
	for k,d in pairs(Program.sectors) do
		if d > 120 and written < 5 then
			-- Group into a single transaction.
			if written == 0 then
				Serialize.db:query("BEGIN TRANSACTION;")
			end
			written = written + 1
			-- Serialize and unload the sector.
			Serialize:save_sector(k)
			Program:unload_sector{sector = k}
		end
	end
	-- Finish the transaction.
	if written > 0 then
		Serialize.db:query("END TRANSACTION;")
	end
end}

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
	local str = "local skills=Skills{owner=self}\n"
	for k,v in pairs(skills:get_names()) do
		local max = skills:get_maximum{skill = v}
		local val = skills:get_value{skill = v}
		local regn = skills:get_regen{skill = v}
		local prot = skills:get_protect{skill = v}
		str = str .. "skills:register{prot=" .. serialize_value(prot) ..
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

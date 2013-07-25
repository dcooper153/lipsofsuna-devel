--- Serializes and deserializes objects.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.object_serializer
-- @alias ObjectSerializer

local Class = require("system/class")
local Serializer = require("system/serializer")

local readers = {
	["base"] = function(self, object, db, create)
	end,
	["fields"] = function(self, object, db, create)
		local rows = db:query(
			[[SELECT name,value FROM object_fields WHERE id=?]], {object:get_id()})
		self.serializer:read(object, rows)
	end,
	["inventory"] = function(self, object, db, create)
		local rows = db:query(
			[[SELECT b.id,b.type,b.spec,b.dead,a.offset,a.slot FROM
			object_inventory AS a INNER JOIN
			object_data AS b WHERE
			a.parent=? AND a.id=b.id]], {object:get_id()})
		for k,v in ipairs(rows) do
			local child = create(v[1], v[2], v[3], v[4])
			if child then
				object.inventory:set_object(v[5], child)
				if v[6] then
					object.inventory:equip_index(v[5], v[6])
				end
			end
		end
	end,
	["parent"] = function(self, object, db, create)
	end,
	["sector"] = function(self, object, db, create)
	end,
	["skills"] = function(self, object, db, create)
		local rows = db:query(
			[[SELECT name FROM object_skills WHERE id=?]], {object:get_id()})
		for k,v in ipairs(rows) do
			object.skills:add_without_requirements(v[1])
		end
		object.skills:remove_invalid()
	end,
	["stats"] = function(self, object, db, create)
		local rows = db:query(
			[[SELECT name,value FROM object_stats WHERE id=?]], {object:get_id()})
		for k,v in ipairs(rows) do
			object.stats:set_value(v[1], v[2])
		end
	end}

local writers = {
	["base"] = function(self, object, db, id)
		db:query([[REPLACE INTO object_data (id,type,spec,dead) VALUES (?,?,?,?);]],
			{id, object:get_storage_type(), object.spec.name, object.dead and 1 or 0})
	end,
	["fields"] = function(self, object, db, id)
		db:query([[DELETE FROM object_fields WHERE id=?;]], {id})
		self.serializer:write(object, function(name, value)
			db:query([[REPLACE INTO object_fields (id,name,value) VALUES (?,?,?);]], {id, name, value})
		end)
	end,
	["inventory"] = function(self, object, db, id)
		db:query([[DELETE FROM object_inventory WHERE parent=?;]], {id})
		for index,child in pairs(object.inventory.stored) do
			child.serializer:write(child, db)
		end
	end,
	["parent"] = function(self, object, db, id)
		local parent = object.parent and Main.objects:find_by_id(object.parent)
		if parent then
			local index = parent.inventory:get_index_by_object(object)
			local slot = parent.inventory:get_slot_by_index(index)
			db:query([[REPLACE INTO object_inventory (id,parent,offset,slot) VALUES (?,?,?,?);]],
				{id, object.parent, index, slot})
		else
			db:query([[DELETE FROM object_inventory WHERE id=?;]], {id})
		end
	end,
	["sector"] = function(self, object, db, id)
		local sector = not object.client and object:get_sector()
		if sector then
			if object.spec.important then
				db:query([[REPLACE INTO object_sectors (id,sector,time) VALUES (?,?,?);]], {id, sector, nil})
			else
				db:query([[REPLACE INTO object_sectors (id,sector,time) VALUES (?,?,?);]], {id, sector, 0})
			end
		else
			db:query([[DELETE FROM object_sectors where id=?;]], {id})
		end
	end,
	["skills"] = function(self, object, db, id)
		db:query([[DELETE FROM object_skills WHERE id=?;]], {id})
		for name,value in pairs(object.skills.skills) do
			db:query([[REPLACE INTO object_skills (id,name) VALUES (?,?);]], {id, name})
		end
	end,
	["stats"] = function(self, object, db, id)
		db:query([[DELETE FROM object_stats WHERE id=?;]], {id})
		for name,args in pairs(object.stats.stats) do
			db:query([[REPLACE INTO object_stats (id,name,value) VALUES (?,?,?);]], {id, name, args.value})
		end
	end}

--- Serializes and deserializes objects.
-- @type ObjectSerializer
local ObjectSerializer = Class("ObjectSerializer")

--- Creates a new database object serializer.
-- @param clss DatabaseObjectSerializer class.
-- @param components List of components to write.
-- @param fields List of serializable fields.
-- @return ObjectSerializer.
ObjectSerializer.new = function(clss, components, fields)
	local self = Class.new(clss)
	self.components = components
	self.serializer = Serializer(fields)
	return self
end

--- Reads the object from the database.
-- @param self ObjectDatabaseSerialize.
-- @param object Object.
-- @param db Database.
-- @param create Function for creating child objects.
ObjectSerializer.read = function(self, object, db, create)
	for k,v in ipairs(self.components) do
		local f = readers[v]
		f(self, object, db, create)
	end
end

--- Writes the object into the database.
-- @param self ObjectDatabaseSerialize.
-- @param object Object.
-- @param db Database.
ObjectSerializer.write = function(self, object, db)
	local id = object:get_id()
	for k,v in ipairs(self.components) do
		local f = writers[v]
		f(self, object, db, id)
	end
end

return ObjectSerializer

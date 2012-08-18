require "system/class"

ObjectDatabase = Class()

--- Creates a new object database.
-- @param clss ObjectDatabase class.
-- @param db Database.
-- @returns ObjectDatabase.
ObjectDatabase.new = function(clss, db)
	local self = Class.new(clss)
	self.db = db
	self.object_decay_timer = 0
	self.object_decay_timeout = 900
	self.object_decay_update = 300
	return self
end

--- Resets the object database.
-- @param self ObjectDatabase.
ObjectDatabase.reset = function(self)
	self.db:query([[DROP TABLE IF EXISTS object_data;]])
	self.db:query([[DROP TABLE IF EXISTS object_inventory;]])
	self.db:query([[DROP TABLE IF EXISTS object_sectors;]])
	self.db:query([[DROP TABLE IF EXISTS object_skills;]])
	self.db:query([[DROP TABLE IF EXISTS object_stats;]])
	self.db:query([[CREATE TABLE object_data (
		id INTEGER PRIMARY KEY,
		type TEXT,
		spec TEXT,
		dead INTEGER,
		data TEXT);]])
	self.db:query([[CREATE TABLE object_inventory (
		id INTEGER PRIMARY KEY,
		parent INTEGER,
		offset INTEGER,
		slot TEXT);]])
	self.db:query([[CREATE TABLE object_sectors (
		id INTEGER PRIMARY KEY,
		sector INTEGER,
		time INTEGER);]])
	self.db:query([[CREATE TABLE object_skills (
		id INTEGER,
		name TEXT,
		PRIMARY KEY(id,name));]])
	self.db:query([[CREATE TABLE object_stats (
		id INTEGER,
		name TEXT,
		value FLOAT,
		PRIMARY KEY(id,name));]])
end

--- Removes all objects from the database.
-- @param self ObjectDatabase.
ObjectDatabase.clear_objects = function(self)
	self.db:query([[DELETE FROM object_data;]])
	self.db:query([[DELETE FROM object_inventory;]])
	self.db:query([[DELETE FROM object_sectors;]])
	self.db:query([[DELETE FROM object_skills;]])
	self.db:query([[DELETE FROM object_stats;]])
end

--- Removes objects that are not in the world map or inventories.
-- @param self ObjectDatabase.
ObjectDatabase.clear_unused_objects = function(self)
	-- Delete objects that are neither in the map or the inventory.
	self.db:query([[DELETE FROM object_data WHERE
		type <> 'static' AND
		(type <> 'player' OR dead = 1) AND
		NOT EXISTS (SELECT 1 FROM object_inventory AS a WHERE object_data.id=a.id) AND
		NOT EXISTS (SELECT 1 FROM object_sectors AS a WHERE object_data.id=a.id);]])
	-- Delete orphaned inventory data.
	-- The fields of the object_inventory table are considered orphaned if
	-- either the object ID or the parent ID are not in object_data
	self.db:query([[DELETE FROM object_inventory WHERE NOT EXISTS
		(SELECT 1 FROM
		object_data AS a INNER JOIN
		object_data AS b WHERE
			a.id=object_inventory.id AND
			b.id=object_inventory.parent);]])
	-- Delete orphaned sector data.
	self.db:query([[DELETE FROM object_sectors WHERE NOT EXISTS
		(SELECT 1 FROM object_data WHERE
			object_data.id=object_sectors.id);]])
end

--- Prevents all world objects from decaying until their sectors have been loaded.
-- @param self ObjectDatabase.
ObjectDatabase.clear_world_decay = function(self)
	self.db:query([[UPDATE object_sectors SET time=?;]], {nil})
end

--- Causes the world objects to decay immediately.
-- @param self ObjectDatabase.
ObjectDatabase.decay_world_now = function(self)
	self:update_world_decay(self.object_decay_timeout)
end

--- Deletes the given object from the database.
-- @param self ObjectDatabase.
-- @param object Object.
ObjectDatabase.delete_object = function(self, object)
	self.db:query([[DELETE FROM object_data WHERE id=?;]], {object.id})
	self.db:query([[DELETE FROM object_inventory WHERE id=?;]], {object.id})
	self.db:query([[DELETE FROM object_sectors WHERE id=?;]], {object.id})
	self.db:query([[DELETE FROM object_skills WHERE id=?;]], {object.id})
	self.db:query([[DELETE FROM object_stats WHERE id=?;]], {object.id})
end

--- Returns true if the object with the given ID exists in the database.
-- @param self ObjectDatabase.
-- @param id Object ID.
ObjectDatabase.does_object_exist = function(self, id)
	local rows = self.db:query([[SELECT id FROM object_data WHERE id=?;]], {id})
	return rows and rows[1]
end

--- Reads all objects in a sector.
-- @param self ObjectDatabase.
-- @param parent Object.
-- @return List of objects.
ObjectDatabase.load_inventory = function(self, parent)
	local objects = {}
	local rows = self.db:query(
		[[SELECT b.id,b.type,b.spec,b.dead,b.data,a.offset,a.slot FROM
		object_inventory AS a INNER JOIN
		object_data AS b WHERE
		a.parent=? AND a.id=b.id]], {parent.id})
	for k,v in ipairs(rows) do
		local obj = self:load_object(v[1], v[2], v[3], v[4], v[5])
		if obj then
			parent.inventory:set_object(v[6], obj)
			if v[7] then
				parent.inventory:equip_index(v[6], v[7])
			end
			table.insert(objects, obj)
		end
	end
	return objects
end

--- Reads an object from the database.
-- @param self ObjectDatabase.
-- @param id Object ID.
-- @param type Object type.
-- @param spec Spec name.
-- @param dead One for dead, zero for alive.
-- @param data Object data.
-- @return Object.
ObjectDatabase.load_object = function(self, id, type, spec, dead, data)
	-- Load the data string.
	if not data then return end
	local func = loadstring("return " .. data)
	if not func then return end
	-- Convert the data string into a table.
	local ok,args = pcall(func)
	if not ok then
		print("ERROR: " .. args)
		return
	end
	-- Add arguements from the queried row.
	args.id = id
	args.dead = (dead == 1) and true or nil
	args.spec = spec
	-- Create the object.
	local init = function(args)
		if type == "actor" then return Actor(args)
		elseif type == "item" then return Item(args)
		elseif type == "obstacle" then return Obstacle(args)
		elseif type == "player" then return Player(args)
		elseif type == "static" then return Staticobject(args)
		else
			error(string.format("invalid object type %q", type))
		end
	end
	local ok1,object = pcall(init, args)
	if not ok1 then
		print("ERROR: " .. object)
		return
	end
	-- Read additional data.
	object:read_db(self.db)
	return object
end

--- Reads a player object from the database.
-- @param self ObjectDatabase.
-- @param account Account.
-- @return Object, or nil.
ObjectDatabase.load_player = function(self, account)
	if not account.character then return end
	local rows = self.db:query(
		[[SELECT id,type,spec,dead,data FROM object_data
		WHERE type=? AND id=?]],
		{"player", account.character})
	for k,v in ipairs(rows) do
		return self:load_object(v[1], v[2], v[3], v[4], v[5])
	end
end

--- Reads the skills of the object from the database.
-- @param self ObjectDatabase.
-- @param parent Object.
ObjectDatabase.load_skills = function(self, parent)
	local rows = self.db:query(
		[[SELECT name FROM object_skills WHERE id=?]], {parent.id})
	for k,v in ipairs(rows) do
		parent.skills:add_without_requirements(v[1])
	end
	parent.skills:remove_invalid()
end

--- Reads the skills of the object from the database.
-- @param self ObjectDatabase.
-- @param parent Object.
ObjectDatabase.load_stats = function(self, parent)
	local rows = self.db:query(
		[[SELECT name,value FROM object_stats WHERE id=?]], {parent.id})
	for k,v in ipairs(rows) do
		parent.stats:set_value(v[1], v[2])
	end
end

--- Reads all objects in a sector.
-- @param self ObjectDatabase.
-- @param sector Sector index.
-- @return List of objects.
ObjectDatabase.load_sector_objects = function(self, sector)
	local objects = {}
	local rows = self.db:query(
		[[SELECT b.id,b.type,b.spec,b.dead,b.data FROM
		object_sectors AS a INNER JOIN
		object_data AS b WHERE
		a.sector=? AND a.id=b.id]], {sector})
	for k,v in ipairs(rows) do
		local obj = self:load_object(v[1], v[2], v[3], v[4], v[5])
		if obj then
			obj:set_visible(true)
			table.insert(objects, obj)
		end
	end
	return objects
end

--- Reads all static objects.
-- @param self ObjectDatabase.
-- @return List of objects.
ObjectDatabase.load_static_objects = function(self)
	local objects = {}
	local rows = self.db:query(
		[[SELECT id,type,spec,dead,data FROM object_data WHERE type=?]], {"static"})
	for k,v in ipairs(rows) do
		local obj = self:load_object(v[1], v[2], v[3], v[4], v[5])
		if obj then
			obj:set_visible(true)
			table.insert(objects, obj)
		end
	end
	return objects
end

--- Writes an object to the database.
-- @param self ObjectDatabase.
-- @param object Object.
ObjectDatabase.save_object = function(self, object)
	object:write_db(self.db)
end

--- Writes objects in the given sector to the database.
-- @param self ObjectDatabase.
-- @param sector Sector number.
ObjectDatabase.save_sector_objects = function(self, sector)
	self.db:query([[DELETE FROM object_sectors WHERE sector=?;]], {sector})
	local objs = SimulationObject:find{sector = sector}
	for k,v in pairs(objs) do
		v:write_db(self.db)
	end
end

--- Writes all static objects to the database.
-- @param self ObjectDatabase.
ObjectDatabase.save_static_objects = function(self)
	for k,v in pairs(Game.static_objects_by_id) do
		if v:get_visible() then
			v:write_db(self.db)
		end
	end
end

--- Updates the age of world objects and removes decayed ones.
-- @param self ObjectDatabase.
-- @param secs Seconds since the last delay, or nil to update immediately.
ObjectDatabase.update_world_decay = function(self, secs)
	-- Only update periodically.
	if not self.object_decay_timer then return end
	if secs then
		self.object_decay_timer = self.object_decay_timer + secs
		if self.object_decay_timer < self.object_decay_update then return end
	end
	-- Increment the age of the objects and delete old objects.
	-- Important objects and obstacles set the time to NULL so this won't affect them.
	self.db:query([[UPDATE object_sectors SET time=time+?;]], {self.object_decay_timer})
	self.db:query([[DELETE FROM object_sectors WHERE time>?;]], {self.object_decay_timeout})
	-- Wait for the next cycle.
	self.object_decay_timer = 0
end

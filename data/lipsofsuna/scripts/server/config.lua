Config = Class()

-- FIXME
Config.tilewidth = 32 / Voxel.tiles_per_line
Config.tilescale = 1 / Config.tilewidth
Config.gravity = Vector(0, -20, 0)
Config.skillregen = 0.5
local reg = Regionspec:find{name = "Lips"}
Config.center = Vector(reg.position[1], reg.depth[1], reg.position[2]) * Config.tilewidth
Config.spawn = Config.center + Vector(17, 7, 17)

--- Creates a new server configuration instance.
-- @param clss Config class.
-- @return Config.
Config.new = function(clss)
	local self = Class.new(clss)
	self.admins = {}
	self.config = ConfigFile{name = "server.cfg"}
	self:load()
	self:save()
	return self
end

--- Loads server configuration.
-- @param self Config.
Config.load = function(self)
	-- Load admins.
	local s = self.config:get("admins")
	if s then
		self.admins = {}
		for w in string.gmatch(s, "[ \t\n]*([a-zA-Z0-9]+)[ \t\n]*") do
			self.admins[w] = true
		end
	end
end

--- Saves server configuration.
-- @param self Config.
Config.save = function(self)
	-- Write admins.
	local a = {}
	for k in pairs(self.admins) do table.insert(a, k) end
	table.sort(a)
	local s = ""
	for k,v in ipairs(a) do s = s .. v end
	self.config:set("admins", s)
	-- Flush the file.
	self.config:save()
end

Config.inst = Config()

require "common/spec"

Species = Class(Spec)
Species.type = "creature"
Species.dict_id = {}
Species.dict_cat = {}
Species.dict_name = {}

--- Creates a copy of the species.
-- @param orig Species.
-- @param args Arguments.
-- @return Species.
Species.copy = function(orig, args)
	local self = Spec.new(Species, args)
	-- Factions.
	self.factions = {}
	for k,v in pairs(orig.factions) do
		self.factions[k] = v
	end
	-- Feats.
	self.feats = {}
	for k,v in pairs(orig.feats) do
		self.feats[k] = v
	end
	-- Slots.
	self.equipment_slots = {}
	for k,v in pairs(orig.equipment_slots) do
		self.equipment_slots[k] = {name = v.name, node = v.node}
	end
	-- Items.
	self.inventory_items = {}
	for k,v in pairs(orig.inventory_items) do
		self.inventory_items[k] = v
	end
	-- Skills.
	self.skills = {}
	for k,v in pairs(orig.skills) do
		self.skills[k] = {max = v.max, name = v.name, val = v.val}
	end
	-- Models.
	if orig.models then
		self.models = {}
		for k,v in pairs(orig.models) do
			self.models[k] = v
		end
	end
	-- Others.
	self.ai_enabled = orig.ai_enabled
	self.ai_enable_attack = self.ai_enable_attack
	self.ai_enable_combat = self.ai_enable_combat
	self.ai_enable_defend = self.ai_enable_defend
	self.ai_enable_strafe = self.ai_enable_strafe
	self.ai_enable_wander = self.ai_enable_wander
	self.ai_search_time = orig.ai_search_time
	self.ai_update_delay = orig.ai_update_delay
	self.ai_wait_allowed = orig.ai_wait_allowed
	self.ai_wander_time = orig.ai_wander_time
	self.dialog = orig.dialog
	self.gravity = orig.gravity
	self.hair_color = orig.hair_color
	self.hair_style = orig.hair_style
	self.inventory_size = orig.inventory_size
	self.mass = orig.mass
	self.model = orig.model
	self.skill_quota = orig.skill_quota
	self.speed_walk = orig.speed_walk
	self.speed_run = orig.speed_run
	return self
end

--- Checks if the object is an enemy of the species.
-- @param self Species.
-- @param object Object.
-- @return True if an enemy.
Species.check_enemy = function(self, object)
	if object.species then
		for name1,spec1 in pairs(self.factions) do
			for name2,spec2 in pairs(object.species.factions) do
				if spec1.enemies[name2] then
					return true
				end
			end
		end
	end
end

--- Registers a new species.
-- @param clss Species class.
-- @param args Arguments.
Species.new = function(clss, args)
	-- Initialization.
	local self
	if args.base then
		base = clss:find{name = args.base}
		self = base:copy{name = args.name}
	else
		self = Spec.new(clss, args)
		self.factions = {}
		self.feats = {}
		self.skills = {}
	end
	-- Factions.
	if args.factions then
		for k,v in pairs(args.factions) do
			self.factions[v] = Faction:find{name = v}
		end
	end
	-- Feats.
	if args.feats then
		for k,v in pairs(args.feats) do
			self.feats[v] = true
		end
	end
	-- Skills.
	if args.skills then
		for k,v in pairs(args.skills) do
			self.skills[v.name] = v
		end
	end
	-- Models.
	if args.models then
		self.models = {}
		for k,v in pairs(args.models) do
			self.models[k] = v
		end
	end
	-- Others.
	local copy = function(f, d)
		if args[f] ~= nil then self[f] = args[f] end
		if self[f] == nil then self[f] = d end
	end
	copy("ai_enable_attack", true)
	copy("ai_enable_combat", true)
	copy("ai_enable_defend", true)
	copy("ai_enable_strafe", true)
	copy("ai_enable_wander", true)
	copy("ai_enable_walk", true)
	copy("ai_enabled", true)
	copy("ai_search_time", 20)
	copy("ai_update_delay", 2)
	copy("ai_wait_allowed", true)
	copy("ai_wander_time", 60)
	copy("dialog")
	copy("equipment_slots", {})
	copy("gravity", Vector(0, -20, 0))
	copy("hair_color")
	copy("hair_style")
	copy("inventory_items", {})
	copy("inventory_size", 0)
	copy("mass", 50)
	copy("model", "aer1")
	self.name = args.name
	copy("skill_quota", 200)
	copy("speed_walk", 3)
	copy("speed_run", 6)
	return self
end

--- Sets the factions of the species.
-- @param self Species.
-- @param args List of species.
Species.set_factions = function(self, args)
	if args then
		self.factions = {}
		for k,v in pairs(args) do
			self.factions[v] = Faction:find{name = v}
		end
	end
end

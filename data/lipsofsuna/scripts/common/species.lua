require "common/spec"

Species = Class(Spec)
Species.type = "species"
Species.dict_id = {}
Species.dict_cat = {}
Species.dict_name = {}

--- Registers a new species.
-- @param clss Species class.
-- @param args Arguments.
Species.new = function(clss, args)
	-- Copy arguments from the base species.
	-- The arguments used to initialize each species have been stored to the
	-- args field of the instance so that they can be easily copied to the
	-- inherited species here.
	if args.base then
		local t = {}
		local base = Species:find{name = args.base}
		for k,v in pairs(base.args) do t[k] = v end
		for k,v in pairs(args) do t[k] = v end
		args = t
	end
	-- Allocate the species.
	-- Many arguments are copied directly from the argument list but some
	-- are reset to empty arrays because they require conversion that is
	-- done later. The argument list is also stored to the species to
	-- allow easy inheritance.
	local self = Spec.new(clss, args)
	self.args = args
	self.equipment_slots = {}
	self.factions = {}
	self.feats = {}
	self.skills = {}
	-- Equipment slots.
	-- Converted from a list to a dictionary to make searching easier.
	if args.equipment_slots then
		for k,v in pairs(args.equipment_slots) do
			self.equipment_slots[v.name] = v
		end
	end
	-- Factions.
	-- Converted from a list to a dictionary to make searching easier.
	-- The faction names are also replaced by the faction objects themselves.
	if args.factions then
		self.factions = {}
		for k,v in pairs(args.factions) do
			self.factions[v] = Faction:find{name = v}
		end
	end
	-- Feats.
	-- Converted from a list to a dictionary to make searching easier.
	-- The key is the name of the feat and the value is unused.
	if args.feats then
		for k,v in pairs(args.feats) do
			self.feats[v] = true
		end
	end
	-- Skills.
	-- Converted from a list to a dictionary to make searching easier.
	if args.skills then
		for k,v in pairs(args.skills) do
			self.skills[v.name] = v
		end
	end
	-- Default values.
	-- To make things easier for the rest of the scripts, some fields are
	-- guaranteed to always have a value. The default values are set here
	-- if no value is assigned to the field yet.
	local copy = function(f, d) if self[f] == nil then self[f] = d end end
	copy("ai_distance_hint", 1)
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
	copy("aim_ray_center", Vector(0, 1, 0))
	copy("aim_ray_end", 5)
	copy("aim_ray_start", 0.1)
	copy("gravity", Vector(0, -15, 0))
	copy("inventory_items", {})
	copy("inventory_size", 0)
	copy("mass", 50)
	copy("skill_quota", 200)
	copy("speed_walk", 3)
	copy("speed_run", 6)
	copy("tilt_limit", math.pi)
	return self
end

--- Checks if the object is an enemy of the species.
-- @param self Species.
-- @param object Object.
-- @return True if an enemy.
Species.check_enemy = function(self, object)
	if object.spec.type == "species" then
		for name1,spec1 in pairs(self.factions) do
			for name2,spec2 in pairs(object.spec.factions) do
				if spec1.enemies[name2] then
					return true
				end
			end
		end
	end
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

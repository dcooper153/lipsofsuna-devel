local Actor = require("core/objects/actor")

Feateffectspec{
	name = "fire elemental",
	categories =
	{
		["spell"] = true,
		["summon"] = true
	},
	actions =
	{
		["self spell"] = true
	},
	description = "Conjure a fire elemental",
	effect = "spell1", --FIXME
	icon = "firewall", --FIXME
	influences = {["fire elemental"] = 60},
	required_stats = {["willpower"] = 10},
	touch = function(self, args)
		-- Find an empty ground spot.
		if not args.object then return end
		local ctr = Utils:find_summon_point(args.object:get_position())
		if not ctr then return end
		-- Create the summon.
		local spec = Actorspec:find{name = "fire elemental"}
		if not spec then return end
		local summon = Actor(args.owner)
		summon:set_spec(spec)
		summon:set_position(ctr)
		summon:randomize()
		summon:set_visible(true)
		summon.summon_owner = args.owner
		summon.summon_timer = args.value
	end}

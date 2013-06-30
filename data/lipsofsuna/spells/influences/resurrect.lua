Feateffectspec{
	name = "resurrect",
	categories =
	{
		["beneficial"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true,
		["touch spell"] = true
	},
	description = "Resurrect the targeted creature",
	effect = "heal1",
	icon = "modifier-heal",
	influences = {["resurrect"] = 1},
	required_stats = {["willpower"] = 20},
	touch = function(self, args)
		if not args.object then return end
		if args.object.class == Player and not args.object.client then
			args.owner:send_message("The corpse is devoid of life force...")
		else
			args.object:action("resurrect")
		end
	end}

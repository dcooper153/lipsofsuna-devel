local Coroutine = require("system/coroutine")
local Physics = require("system/physics")

Actionspec{name = "spell on touch", func = function(feat, info, args)
	Coroutine(function(t)
		feat:play_effects(args)
		Coroutine:sleep(args.user.spec.timing_spell_touch * 0.02)
		local src,dst = args.user:get_attack_ray()
		local r = Physics:cast_ray(src, dst)
		if not r then return end
		r.object = r.object and Game.objects:find_by_id(r.object)
		feat:apply{
			charge = args.charge,
			object = r.object,
			owner = args.user,
			point = r.point,
			tile = r.tile,
			weapon = args.weapon}
	end)
end}

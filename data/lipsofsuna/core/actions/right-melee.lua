local choose_move = function(attacker)
	if attacker:get_strafing() < -0.2 then return "left"
	elseif attacker:get_strafing() > 0.2 then return "right"
	elseif attacker:get_movement() < -0.2 then return "back"
	elseif attacker:get_movement() > 0.2 then return "front"
	else return "stand" end
end

Actionspec{
	name = "right melee",
	charge_start = function(user)
		local move = choose_move(user)
		user:animate("charge " .. move, true)
		user.attack_charge = Program:get_time()
		user.attack_charge_anim = "right melee"
		user.attack_charge_move = move
	end,
	charge_end = function(user)
		user:action("melee", user.attack_charge_move)
		user:attack_charge_cancel()
		user.auto_attack = nil
		user.cooldown = (user.cooldown or 0) + 0.8
	end,
	charge_cancel = function(user)
		user:attack_charge_cancel(true)
		user.auto_attack = nil
		user.cooldown = 0.4
	end,
	func = function(feat, info, args)
		args.user:action("melee", choose_move(args.user))
	end}

Actionspec{
	name = "block",
	charge_start = function(user)
		user:set_block(true)
		user.attack_charge_anim = "block"
	end,
	charge_end = function(user)
		user:set_block(false)
		user:attack_charge_cancel()
	end,
	charge_cancel = function(user)
		user:set_block(false)
		user:attack_charge_cancel()
	end}

Actionspec{
	name = "block weapon",
	charge_start = function(user)
		user:set_block(true)
		user.attack_charge_anim = "block weapon"
	end,
	charge_end = function(user)
		user:set_block(false)
		user:attack_charge_cancel()
	end,
	charge_cancel = function(user)
		user:set_block(false)
		user:attack_charge_cancel()
	end}

Actionspec{
	name = "block shield",
	charge_start = function(user)
		user:set_block(true)
		user.attack_charge_anim = "block shield"
	end,
	charge_end = function(user)
		user:set_block(false)
		user:attack_charge_cancel()
	end,
	charge_cancel = function(user)
		user:set_block(false)
		user:attack_charge_cancel()
	end}

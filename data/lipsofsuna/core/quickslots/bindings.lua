local Client = require("core/client/client")
local Keysym = require("system/keysym")
local Quickslots = require("core/quickslots/quickslots")

Client:register_init_hook(500, function()

	Client.input:register_binding{name = "quickslot_mode", mode = "press", key1 = Keysym.PERIOD, func = function()
		if not Client.player_object then return end
		if Quickslots.mode ~= "feats" then
			Quickslots.mode = "feats"
		else
			Quickslots.mode = "items"
		end
	end}

	Client.input:register_binding{name = "quickslot_1", mode = "press", key1 = Keysym.NUM1, func = function()
		if not Client.player_object then return end
		Quickslots:activate(1)
	end}

	Client.input:register_binding{name = "quickslot_2", mode = "press", key1 = Keysym.NUM2, func = function()
		if Client.player_object then
			Quickslots:activate(2)
		end
	end}

	Client.input:register_binding{name = "quickslot_3", mode = "press", key1 = Keysym.NUM3, func = function()
		if not Client.player_object then return end
		Quickslots:activate(3)
	end}

	Client.input:register_binding{name = "quickslot_4", mode = "press", key1 = Keysym.NUM4, func = function()
		if not Client.player_object then return end
		Quickslots:activate(4)
	end}

	Client.input:register_binding{name = "quickslot_5", mode = "press", key1 = Keysym.NUM5, func = function()
		if not Client.player_object then return end
		Quickslots:activate(5)
	end}

	Client.input:register_binding{name = "quickslot_6", mode = "press", key1 = Keysym.NUM6, func = function()
		if not Client.player_object then return end
		Quickslots:activate(6)
	end}

	Client.input:register_binding{name = "quickslot_7", mode = "press", key1 = Keysym.NUM7, func = function()
		if not Client.player_object then return end
		Quickslots:activate(7)
	end}

	Client.input:register_binding{name = "quickslot_8", mode = "press", key1 = Keysym.NUM8, func = function()
		if not Client.player_object then return end
		Quickslots:activate(8)
	end}

	Client.input:register_binding{name = "quickslot_9", mode = "press", key1 = Keysym.NUM9, func = function()
		if not Client.player_object then return end
		Quickslots:activate(9)
	end}

	Client.input:register_binding{name = "quickslot_10", mode = "press", key1 = Keysym.NUM0, func = function()
		if not Client.player_object then return end
		Quickslots:activate(10)
	end}

end)

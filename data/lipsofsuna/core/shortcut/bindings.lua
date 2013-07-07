local Client = require("core/client/client")
local Keysym = require("system/keysym")

Client:register_init_hook(500, function()

	Client.input:register_binding{name = "shortcut_1", mode = "press", key1 = Keysym.NUM1, func = function()
		Main.shortcuts:activate(1)
	end}

	Client.input:register_binding{name = "shortcut_2", mode = "press", key1 = Keysym.NUM2, func = function()
		Main.shortcuts:activate(2)
	end}

	Client.input:register_binding{name = "shortcut_3", mode = "press", key1 = Keysym.NUM3, func = function()
		Main.shortcuts:activate(3)
	end}

	Client.input:register_binding{name = "shortcut_4", mode = "press", key1 = Keysym.NUM4, func = function()
		Main.shortcuts:activate(4)
	end}

	Client.input:register_binding{name = "shortcut_5", mode = "press", key1 = Keysym.NUM5, func = function()
		Main.shortcuts:activate(5)
	end}

	Client.input:register_binding{name = "shortcut_6", mode = "press", key1 = Keysym.NUM6, func = function()
		Main.shortcuts:activate(6)
	end}

	Client.input:register_binding{name = "shortcut_7", mode = "press", key1 = Keysym.NUM7, func = function()
		Main.shortcuts:activate(7)
	end}

	Client.input:register_binding{name = "shortcut_8", mode = "press", key1 = Keysym.NUM8, func = function()
		Main.shortcuts:activate(8)
	end}

	Client.input:register_binding{name = "shortcut_9", mode = "press", key1 = Keysym.NUM9, func = function()
		Main.shortcuts:activate(9)
	end}

	Client.input:register_binding{name = "shortcut_10", mode = "press", key1 = Keysym.NUM0, func = function()
		Main.shortcuts:activate(10)
	end}

end)

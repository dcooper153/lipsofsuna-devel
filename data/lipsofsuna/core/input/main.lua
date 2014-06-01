local Client = require("core/client/client")
local Eventhandler = require("system/eventhandler")
local Input = require("core/input/input")
local Ui = require("ui/ui")

Main.main_start_hooks:register(110, function()
	Client.input = Input()
	Client.bindings = Client.input.bindings --FIXME
end)

Main.main_start_hooks:register(1000, function()
	Client.input:load()
end)

local handle_input = function(self, args)
	Client.input:event(args)
	if not Main.windows or Main.windows:handle_event(args) then
		Client.input.bindings:event(args)
	end
end

Eventhandler{type = "keypress", func = handle_input}
Eventhandler{type = "keyrelease", func = handle_input}
Eventhandler{type = "mousepress", func = handle_input}
Eventhandler{type = "mouserelease", func = handle_input}
Eventhandler{type = "mousescroll", func = handle_input}
Eventhandler{type = "mousemotion", func = handle_input}

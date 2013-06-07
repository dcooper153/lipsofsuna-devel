local Client = require("core/client/client")
local Program = require("system/core")
local Ui = require("ui/ui")

Client:register_init_hook(0, function(secs)
	Ui:init()
end)

Client:register_update_hook(5, function(secs)
	-- Emit key repeat events.
	local t = Program:get_time()
	for k,v in pairs(Client.input.pressed) do
		if t - v.time > 0.05 then
			v.type = "keyrepeat"
			v.mods = Client.input.mods
			v.time = t
			Ui:handle_event(v)
		end
	end
	-- Update the user interface state.
	Ui:update(secs)
	-- Update the window size.
	if Ui.was_resized then
		local v = Program:get_video_mode()
		Client.options.window_width = v[1]
		Client.options.window_height = v[2]
		Client.options.fullscreen = v[3]
		Client.options.vsync = v[4]
		Client.options:save()
	end
end)

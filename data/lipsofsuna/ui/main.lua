local Client = require("core/client/client")
local Graphics = require("system/graphics")
local Program = require("system/core")
local Time = require("system/time")
local Ui = require("ui/ui")

Main.main_start_hooks:register(110, function()
	Ui:init()
end)

Client:register_update_hook(5, function(secs)
	Main.timing:start_action("ui")
	-- Emit key repeat events.
	local t = Time:get_secs()
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
		local v = Graphics:get_video_mode()
		Client.options.window_width = v[1]
		Client.options.window_height = v[2]
		Client.options.fullscreen = v[3]
		Client.options.vsync = v[4]
		Client.options:save()
	end
end)

local Client = require("core/client/client")
local EffectManager = require("core/effect/effect-manager")
local Eventhandler = require("system/eventhandler")

Client:register_init_hook(0, function()
	Client.effects = EffectManager()
end)

Client:register_speech_hook(0, function(args)
	Client.effects:speech(args)
end)

Client:register_update_hook(40, function(secs)
	-- Update effects.
	-- Must be done after objects to ensure correct anchoring.
	Client.effects:update(secs)
end)

Main.game_end_hooks:register(40, function()
	if Client.effects then
		Client.effects:detach_scene_nodes()
	end
end)

Eventhandler{type = "music-ended", func = function(self, args)
	Client.effects:cycle_music_track()
end}

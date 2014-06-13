local Arena = require("arena/arena")
local ArenaCamera = require("arena/camera")
local Client = require("core/client/client")
local Ui = require("ui/ui")

Main.game_modes:register("Arena", function()
	Main.arena = Arena()
	Ui:set_state("arena")
end)

Main.game_end_hooks:register(0, function(secs)
	if Main.arena then
		Main.arena:close()
		Main.arena = nil
	end
end)

Main.main_start_hooks:register(130, function(secs)
	Main.objects.object_created_hooks:register(15, function(object)
		--FIXME
		if Main.arena then
			object.has_server_data = function() return true end
		end
	end)
	Client.camera_manager:register_camera("arena", ArenaCamera())
end)

Main.update_hooks:register(0, function(secs)
	if Main.arena then
		Main.timing:start_action("arena")
		Main.arena:update(secs)
	end
end)

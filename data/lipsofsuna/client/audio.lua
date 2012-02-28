Sound.music_mode = "none"

--- Switches the music track.
-- @param clss Sound class.
-- @param mode Music mode. ("boss"/"char"/"game"/"menu")
Sound.switch_music_track = function(clss, mode)
	if not mode then return end
	if mode == clss.music_mode then return end
	clss.music_mode = mode
	clss:cycle_music_track()
end

--- Switches the music track.
-- @param clss Sound class.
-- @param mode Music mode. ("boss"/"char"/"game"/"menu")
Sound.cycle_music_track = function(clss)
	local modes = {
		boss = {"fairytale7", "fairytale9"},
		char = {"fairytale2", "fairytale10", "fairytale8"},
		game = {"fairytale1", "fairytale3", "fairytale4", "fairytale5",
		        "fairytale6", "fairytale11", "fairytale12", "fairytale13"},
		intro = {"fairytale7"},
		menu = {"menu1"}};
	if not clss.music_mode then return end
	local tracks = modes[clss.music_mode];
	if not tracks then return end
	clss.music_fading = 2.0
	clss.music_looping = (#tracks > 1)
	clss.music_volume = Client.options.music_volume
	clss.music = tracks[math.random(1, #tracks)]
end

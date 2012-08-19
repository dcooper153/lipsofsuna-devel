require(Mod.path .. "theme")
require(Mod.path .. "ui")

Program:add_path(Mod.path)
Program:add_path(Mod.path .. "sounds")

Effectspec{
	name = "uimove1",
	sound = "toggle1",
	sound_volume = 0.3}

Effectspec{
	name = "uislider1",
	sound = "slider1",
	sound_volume = 0.3}

Effectspec{
	name = "uitoggle1",
	sound = "toggle1",
	sound_volume = 0.5}

Effectspec{
	name = "uitoggle2",
	sound = "toggle2",
	sound_volume = 0.5}

Effectspec{
	name = "uitransition1",
	sound = "transition1",
	sound_volume = 0.3}

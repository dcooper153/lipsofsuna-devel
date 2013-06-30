local File = require("system/file")
local Program = require("system/core")

Program:add_path(Mod.path .. "effects")
File:require_directory(Mod.path .. "modifiers")

Spellspec{
	name = "fireball1",
	model = "fireball1",
	particle = "fireball1"}

Spellspec{
	name = "firebreath1",
	particle = "firebreath1"}

Spellspec{
	name = "firewall1",
	model = "firewall1",
	particle = "firewall1"}

Spellspec{
	name = "magicmissile1",
	model = "fireball1",
	particle = "fireball1"}

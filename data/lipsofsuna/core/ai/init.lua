if not Settings then return end

local Ai = require(Mod.path .. "ai")
Ai:register(require(Mod.path .. "anchored"))
Ai:register(require(Mod.path .. "npc"))
Ai:register(require(Mod.path .. "camper"))

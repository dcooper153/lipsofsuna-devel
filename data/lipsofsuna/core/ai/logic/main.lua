local Ai = require("core/ai/logic/ai")
local AiManager = require("core/ai/ai-manager")

Ai:register(require("core/ai/logic/anchored"))
Ai:register(require("core/ai/logic/npc"))
Ai:register(require("core/ai/logic/camper"))
Ai:register(require("core/ai/logic/companion"))

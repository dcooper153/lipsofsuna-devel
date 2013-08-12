local ChatCommand = require("core/chat/chat-command")
local Quaternion = require("system/math/quaternion")
local UiScrollFloat = require("ui/widgets/scrollfloat")

local rot = Quaternion()

local update = function()
	local p = Client.player_object
	if not p then return end
	if not p.render then return end
	if not p.render.special_effects then return end
	for k,v in pairs(p.render.special_effects) do
		local r = rot:copy():normalize()
		v.rotation_local = r
		print(r)
	end
end

------------------------------------------------------------------------------

ChatCommand{
	name = "anchor_editor",
	description = "Edit the anchor of the wielded object.",
	pattern = "^/anchor_editor$",
	permission = "player",
	handler = "client",
	func = function(player, matches)
		Ui:set_state("editor/anchor")
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "editor/anchor",
	label = "Anchor editor"}

Ui:add_widget{
	state = "editor/anchor",
	widget = function()
		return
		{
			UiScrollFloat("Rotation X", -1, 1, rot.x, function(w)
				rot.x = w.value
				update()
			end),
			UiScrollFloat("Rotation Y", -1, 1, rot.y, function(w)
				rot.y = w.value
				update()
			end),
			UiScrollFloat("Rotation Z", -1, 1, rot.z, function(w)
				rot.z = w.value
				update()
			end),
			UiScrollFloat("Rotation W", -1, 1, rot.w, function(w)
				rot.w = w.value
				update()
			end)
		}
	end}


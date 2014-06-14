local ModifierSpec = require("core/specs/modifier")

local HomeModifier = Main.specs:find_by_name("ModifierSpec", "home")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
HomeModifier.start = function(modifier, value)
	if not modifier.object then return end
	if not modifier.object.account then return end
	local home = modifier.object:get_position()
	modifier.object.account.spawn_point = home
	Main.messaging:server_event("create-marker", "home", home)
end

Protocol:add_handler{type = "MODIFIER_ADD", func = function(event)
	local ok,n,t = event.packet:read("string", "float")
	if not ok then return end
	Client.data.modifiers[n] = t
	local hud = Ui:get_hud("modifier")
	if hud then hud.widget:add(n, t) end
end}

Protocol:add_handler{type = "MODIFIER_REMOVE", func = function(event)
	local ok,n = event.packet:read("string")
	if not ok then return end
	Client.data.modifiers[n] = nil
	local hud = Ui:get_hud("modifier")
	if hud then hud.widget:remove(n) end
end}

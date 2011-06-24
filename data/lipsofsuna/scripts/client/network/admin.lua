Protocol:add_handler{type = "ADMIN_PRIVILEGE", func = function(event)
	local ok,b = event.packet:read("bool")
	if not ok then return end
	Gui:set_admin(b)
end}

-- Updates the weight and burdening limit of the player.
Protocol:add_handler{type = "ADMIN_STATS", func = function(event)
	local ok,t = event.packet:read("string")
	if not ok then return end
	Client.views.admin.label.text = t
end}

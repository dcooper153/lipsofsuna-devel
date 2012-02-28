Protocol:add_handler{type = "ADMIN_PRIVILEGE", func = function(event)
	local ok,b = event.packet:read("bool")
	if not ok then return end
	Client.data.admin.enabled = b
end}

Protocol:add_handler{type = "ADMIN_STATS", func = function(event)
	local ok,t = event.packet:read("string")
	if not ok then return end
	Client.data.admin.server_stats = t
	Ui.state = "admin/server-stats"
end}

-- Adds a map marker.
Protocol:add_handler{type = "MARKER_ADD", func = function(event)
	local ok,n,x,y,z = event.packet:read("string", "float", "float", "float")
	if not ok then return end
	local m = Marker:find{name = n}
	if not m then
		m = Marker{name = n}
		if not Operators.play:is_startup_period() then
			Client:append_log("Location added: " .. n)
		end
	end
	m.position = Vector(x,y,z)
end}

-- Updates a map marker.
Protocol:add_handler{type = "MARKER_UPDATE", func = function(event)
	local ok,n,x,y,z = event.packet:read("string", "float", "float", "float")
	if not ok then return end
	local m = Marker:find{name = n}
	if not m then return end
	m.position = Vector(x,y,z)
end}

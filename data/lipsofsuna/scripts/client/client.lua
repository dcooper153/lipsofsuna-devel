Program.window_title = "Lips of Suna"
Reload.enabled = true

Eventhandler{type = "select", func = function(self, args)
	args.object.selected = not args.object.selected
end}

Eventhandler{type = "object-model", func = function(self, args)
	--args.object:update_model{model = args.model}
end}

Protocol:add_handler{type = "ADMIN_PRIVILEGE", func = function(event)
	local ok,b = event.packet:read("bool")
	if not ok then return end
	Gui:set_admin(b)
end}

Protocol:add_handler{type = "FEAT_UNLOCK", func = function(event)
	local ok,n = event.packet:read("string")
	if ok then
		local feat = Feat:find{name = n}
		if not feat then return end
		Gui.chat_history:append{text = "Unlocked feat " .. feat.name}
		feat.locked = false
	end
end}

Protocol:add_handler{type = "MESSAGE", func = function(event)
	local ok,msg = event.packet:read("string")
	if ok then
		Gui.chat_history:append{text = msg}
	end
end}

Protocol:add_handler{type = "VOXEL_DIFF", func = function(event)
	Voxel:set_block{packet = event.packet}
end}

Protocol:add_handler{type = "EFFECT_WORLD", func = function(event)
	local ok,t,x,y,z = event.packet:read("string", "float", "float", "float")
	if ok then
		Effect:play_world(t, Vector(x,y,z))
	end
end}

Quickslots = Class()
Quickslots.icons = {}

Quickslots.init = function(clss)
	clss.group = Widget{rows = 1, spacings = {0,0}}
	clss.buttons = {}
	for i = 1,12 do
		clss.buttons[i] = Widgets.Quickslot{pressed = function() clss:activate(i) end}
		clss.group:append_col(clss.buttons[i])
		if i == 4 or i == 8 then
			local pad = Widget()
			pad:set_request{width = 15}
			clss.group:append_col(pad)
		end
	end
end

Quickslots.assign_none = function(clss, index)
	clss.buttons[index].feat = nil
	clss.buttons[index].icon = Iconspec:find{name = "skill-none"}
end

Quickslots.assign_feat = function(clss, index, feat)
	local icon = nil
	if feat then
		for i = 1,3 do
			local effect = feat.effects[i]
			if effect then
				spec = Feateffectspec:find{name = effect[1]}
				icon = spec and Iconspec:find{name = spec.icon}
				if icon then break end
			end
		end
	end
	clss.buttons[index].feat = feat
	clss.buttons[index].icon = icon
end

Quickslots.activate = function(clss, index)
	local feat = clss.buttons[index].feat
	if feat then
		local names = {}
		local values = {}
		for i = 1,3 do
			names[i] = feat.effects[i] and feat.effects[i][1] or ""
			values[i] = feat.effects[i] and feat.effects[i][2] or 0
		end
		Network:send{packet = Packet(packets.FEAT,
			"string", feat.animation or "",
			"string", names[1], "float", values[1],
			"string", names[2], "float", values[2],
			"string", names[3], "float", values[3],
			"bool", true)}
		Network:send{packet = Packet(packets.FEAT,
			"string", feat.animation or "",
			"string", names[1], "float", values[1],
			"string", names[2], "float", values[2],
			"string", names[3], "float", values[3],
			"bool", false)}
	end
end

Quickslots.reset = function(clss)
	clss:assign_feat(1, Feat{animation = "spell on self", effects = {{"restore health", 10}}})
	clss:assign_feat(2, Feat{animation = "ranged spell", effects = {{"fire damage", 10}}})
	clss:assign_feat(3, Feat{animation = "ranged", effects = {{"physical damage", 1}}})
end

Quickslots:init()

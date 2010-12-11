Quickslots = Class()
Quickslots.icons = {}

Quickslots.init = function(clss)
	clss.group = Widget{rows = 1}
	clss.buttons = {}
	for i = 1,12 do
		clss.buttons[i] = Widgets.Icon{
			icon = Iconspec:find{name = "skill-none"},
			pressed = function() clss:activate(i) end}
		clss.group:append_col(clss.buttons[i])
	end
end

Quickslots.assign_none = function(clss, index)
	clss.buttons[index].feat = nil
	clss.buttons[index].icon = Iconspec:find{name = "skill-none"}
end

Quickslots.assign_feat = function(clss, index, feat)
	local icon
	if feat then
		local anim = feat and Featanimspec:find{name = feat.animation}
		icon = anim and Iconspec:find{name = anim.icon}
		icon = icon or Iconspec:find{name = "skill-todo"}
	else
		icon = Iconspec:find{name = "skill-none"}
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

Quickslots:init()
Quickslots:assign_feat(1, Feat{animation = "spell on self", effects = {{"restore health", 10}}})
Quickslots:assign_feat(2, Feat{animation = "ranged spell", effects = {{"fire damage", 10}}})
Quickslots:assign_feat(3, Feat{animation = "ranged", effects = {{"physical damage", 1}}})

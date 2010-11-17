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
		icon = Iconspec:find{name = "skill-" .. feat}
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
		Network:send{packet = Packet(packets.SKILL, "string", feat, "bool", true)}
		Network:send{packet = Packet(packets.SKILL, "string", feat, "bool", false)}
	end
end

Quickslots:init()
local i = 1
for k,v in pairs(Feat.dict_name) do
	if i < 12 then
		Quickslots:assign_feat(i, k)
		i = i + 1
	end
end

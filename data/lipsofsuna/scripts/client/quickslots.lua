Quickslots = Class()
Quickslots.icons = {}

Quickslots.init = function(clss)
	clss.group = Widget{rows = 1}
	clss.buttons = {}
	for i = 1,12 do
		clss.buttons[i] = Widgets.Icon{
			icon = clss.icons["none"],
			pressed = function() clss:activate(i) end}
		clss.group:append_col(clss.buttons[i])
	end
end

Quickslots.assign_none = function(clss, index)
	clss.buttons[index].feat = nil
	clss.buttons[index].icon = clss.icons["none"]
end

Quickslots.assign_feat = function(clss, index, feat)
	local icon = feat and (clss.icons[feat] or clss.icons["todo"]) or clss.icons["none"]
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

local mkicon = function(name)
	local x = math.mod(#Quickslots.icons * 32, 256)
	local y = math.floor(#Quickslots.icons / 32)
	local icon = { image = "skills1", name = name, offset = {x,y}, size = {32,32} }
	Quickslots.icons[name] = icon
	table.insert(Quickslots.icons, icon)
end
mkicon("none")
mkicon("heal")
mkicon("fireball")
mkicon("todo")

Quickslots:init()
local i = 1
for k,v in pairs(Feat.dict_name) do
	if i < 12 then
		Quickslots:assign_feat(i, k)
		i = i + 1
	end
end

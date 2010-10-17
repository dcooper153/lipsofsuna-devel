Quickslots = Class()

Quickslots.init = function(clss)
	clss.group = Group{rows = 1}
	clss.buttons = {}
	for i = 1,12 do
		clss.buttons[i] = Image{
			style = "skill-icon-none",
			pressed = function() clss:activate(i) end}
		clss.group:append_col(clss.buttons[i])
	end
end

Quickslots.assign_none = function(clss, index)
	clss.buttons[index].feat = nil
	clss.buttons[index].style = "skill-icon-none"
end

local icons = {}
Quickslots.assign_feat = function(clss, index, feat)
	local icon = icons[feat] and feat or "todo"
	clss.buttons[index].feat = feat
	clss.buttons[index].style = "skill-icon-" .. icon
end

Quickslots.activate = function(clss, index)
	local feat = clss.buttons[index].feat
	if feat then
		Network:send{packet = Packet(packets.SKILL, "string", feat, "bool", true)}
		Network:send{packet = Packet(packets.SKILL, "string", feat, "bool", false)}
	end
end

local mkicon = function(name)
	local x = math.mod(#icons * 32, 256)
	local y = math.floor(#icons / 32)
	Widgets:add_widget_style("skill-icon-" .. name, [[
		file: skills1.dds;
		source: ]] .. x .. " " .. y .. [[;
		width: 0 32 0;
		height: 0 32 0;
		padding: 16 16 16 16;
	]])
	table.insert(icons, name)
	icons[name] = true
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

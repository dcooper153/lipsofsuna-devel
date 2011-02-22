Quickslots = Class()
Quickslots.icons = {}

Quickslots.init = function(clss)
	clss.group = Widget{rows = 1, spacings = {0,0}}
	clss.buttons = {}
	for i = 1,12 do
		clss.buttons[i] = Widgets.Quickslot{pressed = function(w, a)
			if a.button == 3 then
				clss:assign_none(i)
			else
				clss:activate(i)
			end
		end}
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
	clss.buttons[index].item = nil
	clss.buttons[index].icon = nil
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
	clss.buttons[index].item = nil
	clss.buttons[index].icon = icon
end

--- Assigns an item to a quickslot.
-- @param clss Quickslots class.
-- @param index Quickslot index number.
-- @param name Name of the item spec to be assigned.
Quickslots.assign_item = function(clss, index, name)
	-- Find the icon.
	local icon
	local spec = Itemspec:find{name = name}
	if spec then icon = Iconspec:find{name = spec.icon} end
	if not icon then icon = Iconspec:find{name = "missing1"} end
	-- Assign the item.
	clss.buttons[index].feat = nil
	clss.buttons[index].item = name
	clss.buttons[index].icon = icon
end

--- Called when the quickslot is clicked or accessed by a hotkey.<br/>
-- If a quickslot is activated while dragging an item, the item is assigned
-- to the slot. Otherwise, the item or feat in the slot is activated.
-- @param clss Quickslots class.
-- @param index Quickslot index number.
Quickslots.activate = function(clss, index)
	if Drag:clicked_quickslot(index) then return end
	local feat = clss.buttons[index].feat
	local item = clss.buttons[index].item
	if feat then
		-- Activate a feat.
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
	elseif item then
		-- Use an item.
		local cont = Views.Inventory.inst.container
		if not cont then return end
		local slot = cont:find_item{name = item}
		if not slot then return end
		Commands:use(cont.id, slot)
	end
end

Quickslots.reset = function(clss)
	clss:assign_feat(1, Feat{animation = "spell on self", effects = {{"restore health", 10}}})
	clss:assign_feat(2, Feat{animation = "ranged spell", effects = {{"fire damage", 10}}})
	clss:assign_feat(3, Feat{animation = "ranged", effects = {{"physical damage", 1}}})
end

Quickslots:init()

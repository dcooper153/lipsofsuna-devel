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
	-- Load the quickslots.
	clss.db = Client.db
	clss.db:query("CREATE TABLE IF NOT EXISTS quickslots (slot INTEGER PRIMARY KEY,item TEXT,feat TEXT);")
	clss:load()
end

Quickslots.assign_none = function(clss, index)
	clss.buttons[index].feat = nil
	clss.buttons[index].item = nil
	clss.buttons[index].icon = nil
	clss.buttons[index].tooltip = nil
	clss:save()
end

Quickslots.assign_feat = function(clss, index, feat)
	local icon = feat:get_icon()
	clss.buttons[index].feat = feat
	clss.buttons[index].item = nil
	clss.buttons[index].icon = icon
	clss.buttons[index].tooltip = Widgets.Feattooltip{feat = feat}
	clss:save()
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
	clss.buttons[index].tooltip = Widgets.Itemtooltip{spec = spec}
	clss:save()
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

Quickslots.load = function(clss)
	clss.loading = true
	-- First set defaults.
	clss:assign_feat(1, Feat{animation = "spell on self", effects = {{"restore health", 10}}})
	clss:assign_feat(2, Feat{animation = "ranged spell", effects = {{"fire damage", 10}}})
	-- Load from the database.
	local rows = clss.db:query("SELECT slot,item,feat FROM quickslots;")
	for k,v in ipairs(rows) do
		if v[1] >= 1 and v[1] <= 12 then
			if v[2] and v[2] ~= "" then
				clss:assign_item(v[1], v[2])
			elseif v[3] and v[3] ~= "" then
				clss:assign_feat(v[1], Feat:load{data = v[3]})
			else
				clss:assign_none(v[1])
			end
		end
	end
	clss.loading = nil
end

Quickslots.save = function(clss)
	if clss.loading then return end
	for k,v in ipairs(clss.buttons) do
		local feat = v.feat and v.feat:write()
		local item = v.item
		clss.db:query("REPLACE INTO quickslots (slot,item,feat) VALUES (?,?,?);", {k, item, feat})
	end
end

Quickslots:init()

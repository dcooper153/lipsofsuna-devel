Crafting = Class()

--- Asks the server to craft an item.
-- @param clss Crafting class.
-- @param args Arguments.<ul>
--   <li>index: List index.</li>
--   <li>name: Item name.</li></ul>
Crafting.craft = function(clss, args)
	local name = args.name or Crafting.tree:get_row{row = args.index}
	if name then
		local p = Packet(packets.CRAFTING, "string", name)
		Network:send{packet = p}
	end
end

--- Initializes the crafting UI.
-- @param clss Crafting class.
Crafting.init = function(clss)
	clss.dialog = Widgets.Popup{cols = 1}
	clss.dialog:set_expand{col = 1, row = 2}
	clss.title = Widgets.Label{font = "medium", text = "Crafting"}
	clss.tree = Widgets.List()
	clss.tree:set_request{width = 200, height = 200}
	clss.button = Widgets.Button{text = "Close", pressed = function() Crafting.dialog.floating = false end}
	clss.dialog:append_row(clss.title)
	clss.dialog:append_row(clss.tree)
	clss.dialog:append_row(clss.button)
end

--- Shows the crafting UI.
-- @param clss Crafting class.
-- @param args Arguments.<ul>
--   <li>items: List of craftable items.</li></ul>
Crafting.show = function(clss, args)
	clss.tree:clear()
	local index = 1
	local sorted = {}
	for k,v in pairs(args.items) do
		table.insert(sorted, v)
	end
	table.sort(sorted)
	for k,v in pairs(sorted) do
		local spec = Itemspec:find{name = v}
		clss.tree:append{widget = Widgets.ItemButton{index = index, icon = spec and spec.icon,
			text = v, pressed = function(self) Crafting:craft{index = self.index} end}}
		index = index + 1
	end
	clss.dialog.floating = true
end

Crafting:init()

-- Handle crafting packets.
Protocol:add_handler{type = "CRAFTING", func = function(args)
	local idx = 1
	local items = {}
	local ok,item = args.packet:read("uint8")
	while ok do
		ok,item = args.packet:resume("string")
		if ok then
			items[idx] = item
			idx = idx + 1
		end
	end
	Crafting:show{title = title, items = items}
end}

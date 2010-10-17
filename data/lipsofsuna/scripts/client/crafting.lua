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
	clss.dialog = Group{cols = 1, style = "window"}
	clss.dialog:set_expand{col = 1, row = 2}
	clss.title = Button{text = "Crafting", style = "inventory-label"}
	clss.tree = Widgets.List{pressed = function(tree, row) Crafting:craft{index = row} end}
	clss.tree:set_request{width = 200, height = 200}
	clss.button = Button{text = "Close", pressed = function() Crafting.dialog.floating = false end}
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
	for k,v in pairs(args.items) do
		clss.tree:append{text = v}
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

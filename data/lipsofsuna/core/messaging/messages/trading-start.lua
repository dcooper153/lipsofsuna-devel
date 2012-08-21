Message{
	name = "trading start",
	server_to_client_encode = function(self, items)
		local args = {"uint8", #items}
		for k,v in ipairs(items) do
			table.insert(args, "string")
			table.insert(args, v)
		end
		return args
	end,
	server_to_client_decode = function(self, packet)
		local ok,count = packet:read("uint8")
		if not ok then return end
		local items = {}
		for i=1,count do
			local ok,name = packet:resume("string")
			if not ok then break end
			table.insert(items, name)
		end
		return {items}
	end,
	server_to_client_handle = function(self, items)
		-- Clear the shop.
		Client.data.trading = {buy = {}, sell = {}, shop = {}}
		-- Add the shop items.
		for k,v in ipairs(items) do
			local spec = Itemspec:find{name = v}
			local item = {spec = spec, count = 1}
			Client.data.trading.shop[k] = item
		end
		-- Update the user interface.
		Ui:set_state("trading")
	end}

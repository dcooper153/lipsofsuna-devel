Message{
	name = "update trading",
	client_to_server_encode = function(self, buy, sell)
		local data = {"uint8", #buy, "uint8", #sell}
		for k,v in ipairs(buy) do
			table.insert(data, "uint32")
			table.insert(data, v[1])
			table.insert(data, "uint32")
			table.insert(data, v[2])
		end
		for k,v in ipairs(sell) do
			table.insert(data, "uint32")
			table.insert(data, v[1])
			table.insert(data, "uint32")
			table.insert(data, v[2])
		end
		return data
	end,
	client_to_server_decode = function(self, packet)
		-- Read item counts info.
		local ok,num_buy,num_sell = packet:read("uint8", "uint8")
		if not ok then return end
		-- Read bought items.
		local buy = {}
		for i = 1,num_buy do
			local ok,index,count = packet:resume("uint32", "uint32")
			if ok then
				local item = player.trading.shop[index]
				if item then
					table.insert(player.trading.buy, {index, count})
				end
			end
		end
		-- Read sold items.
		local sell = {}
		for i = 1,num_sell do
			local ok,index,count = packet:resume("uint32", "uint32")
			if ok then
				local item = player.inventory:get_object_by_index(index)
				if item then
					count = math.max(item:get_count(), count)
					table.insert(player.trading.sell, {index, count})
				end
			end
		end
		return {buy, sell}
	end,
	client_to_server_handle = function(self, client, buy, sell)
		-- Find the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		if not player.trading then return end
		-- Update the deal status.
		player.trading.buy = buy
		player.trading.sell = sell
		Server.trading:update(player)
	end}

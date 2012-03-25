Trading = Class()

--- Applies the trade if it's acceptable.
-- @param clss Trading.
-- @param player Player.
Trading.accept = function(clss, player)
	-- Check for an acceptable deal.
	if not player.trading then return end
	if not clss:deal(player) then return end
	-- Remove the sold items.
	for k,v in pairs(player.trading.sell) do
		player.inventory:subtract_objects_by_index(v[1], v[2])
	end
	-- Give the bought items.
	for k,v in pairs(player.trading.buy) do
		local name = player.trading.shop[v[1]]
		local spec = Itemspec:find{name = name}
		local item = Item{spec = spec, count = v[2]}
		player.inventory:merge_or_drop_object(item)
	end
	-- Close the trading screen.
	player.trading = nil
	player:send(Packet(packets.TRADING_CLOSE))
end

--- Cancels the trade.
-- @param clss Trading.
-- @param player Player.
Trading.cancel = function(clss, player)
	if not player.trading then return end
	player:send(Packet(packets.TRADING_CLOSE))
	player.trading = nil
end

--- Returns true if the trade is acceptable.</br>
--
-- This function also takes care of validating the items of the trade in case
-- the player inventory or other conditions have changed. Any invalid items
-- are removed and invalid item counts clamped to the valid bounds.
--
-- @param clss Trading.
-- @param player Player.
Trading.deal = function(clss, player)
	-- Calculate the value of the items sold by the player.
	local sell = 0
	for k,v in pairs(player.trading.sell) do
		local item = player.inventory:get_object_by_index(v[1])
		if item then
			v[2] = math.max(v[2], item.count)
			sell = sell + v[2] * item.spec:get_trading_value()
		else
			player.trading.sell[k] = nil
		end
	end
	-- Calculate the value of the items bought by the player.
	local buy = 0
	for k,v in pairs(player.trading.buy) do
		local name = player.trading.shop[v[1]]
		if name then
			local spec = Itemspec:find{name = name}
			buy = buy + v[2] * spec:get_trading_value()
		else
			player.trading.shop[v[1]] = nil
		end
	end
	-- Accept if the merchant profits.
	-- TODO: Should merchants be more favorable to the same race?
	-- TODO: Should there be other discriminative conditions?
	return 1.5 * sell >= buy
end

--- Starts a trade.
-- @param clss Trading.
-- @param player Player.
-- @param merchant Actor.
Trading.start = function(clss, player, merchant)
	-- Initialize the trading data.
	clss:cancel(player)
	player.trading = {sell = {}, buy = {}, shop = {}, merchant = merchant}
	-- Create the shop list.
	-- TODO: Should depend on the merchant.
	local count = 20
	local data = {}
	for i = 1,count do
		local s = Itemspec:random()
		table.insert(player.trading.shop, s.name)
		table.insert(data, "string")
		table.insert(data, s.name)
	end
	-- Send the trading start packet.
	player:send(Packet(packets.TRADING_START, "uint8", count, unpack(data)))
end

--- Sends a deal status update to the player.
-- @param clss Trading.
-- @param player Player.
Trading.update = function(clss, player)
	player:send(Packet(packets.TRADING_ACCEPT, "bool", clss:deal(player)))
end

Protocol:add_handler{type = "TRADING_ACCEPT", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	if not player.trading then return end
	-- Accept trading.
	Trading:accept(player)
end}

Protocol:add_handler{type = "TRADING_CANCEL", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	if not player.trading then return end
	-- Cancel trading.
	Trading:cancel(player)
end}

Protocol:add_handler{type = "TRADING_UPDATE", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	if not player.trading then return end
	-- Read item counts info.
	local ok,buy,sell = args.packet:read("uint8", "uint8")
	if not ok then return end
	-- Read bought items.
	player.trading.buy = {}
	for i = 1,buy do
		local ok,index,count = args.packet:resume("uint32", "uint32")
		if ok then
			local item = player.trading.shop[index]
			if item then
				table.insert(player.trading.buy, {index, count})
			end
		end
	end
	-- Read sold items.
	player.trading.sell = {}
	for i = 1,sell do
		local ok,index,count = args.packet:resume("uint32", "uint32")
		if ok then
			local item = player.inventory:get_object_by_index(index)
			if item then
				count = math.max(item.count, count)
				table.insert(player.trading.sell, {index, count})
			end
		end
	end
	-- Update the deal status.
	Trading:update(player)
end}

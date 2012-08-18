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
	Game.messaging:server_event("trading end", player.client)
end

--- Cancels the trade.
-- @param clss Trading.
-- @param player Player.
Trading.cancel = function(clss, player)
	if not player.trading then return end
	Game.messaging:server_event("trading end", player.client)
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
	for i = 1,count do
		local s = Itemspec:random()
		table.insert(player.trading.shop, s.name)
	end
	-- Send the trading start packet.
	Game.messaging:server_event("trading start", player.client, player.trading.shop)
end

--- Sends a deal status update to the player.
-- @param clss Trading.
-- @param player Player.
Trading.update = function(clss, player)
	Game.messaging:server_event("trading accept", player.client, clss:deal(player))
end

--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.trading
-- @alias Trading

local Class = require("system/class")

--- TODO:doc
-- @type Trading
local Trading = Class("Trading")

Trading.new = function(clss)
	local self = Class.new(clss)
	return clss
end

--- Applies the trade if it's acceptable.
-- @param self Trading.
-- @param player Player.
Trading.accept = function(self, player)
	-- Check for an acceptable deal.
	if not player.trading then return end
	if not self:deal(player) then return end
	-- Remove the sold items.
	for k,v in pairs(player.trading.sell) do
		player.inventory:subtract_objects_by_index(v[1], v[2])
	end
	-- Give the bought items.
	for k,v in pairs(player.trading.buy) do
		local name = player.trading.shop[v[1]]
		local item = player.manager:create_object_by_spec("Item", name)
		if item then
			item:set_count(v[2])
			player.inventory:merge_or_drop_object(item)
		end
	end
	-- Close the trading screen.
	player.trading = nil
	Main.messaging:server_event("trading end", player.client)
end

--- Cancels the trade.
-- @param self Trading.
-- @param player Player.
Trading.cancel = function(self, player)
	if not player.trading then return end
	Main.messaging:server_event("trading end", player.client)
	player.trading = nil
end

--- Returns true if the trade is acceptable.</br>
--
-- This function also takes care of validating the items of the trade in case
-- the player inventory or other conditions have changed. Any invalid items
-- are removed and invalid item counts clamped to the valid bounds.
--
-- @param self Trading.
-- @param player Player.
Trading.deal = function(self, player)
	-- Calculate the value of the items sold by the player.
	local sell = 0
	for k,v in pairs(player.trading.sell) do
		local item = player.inventory:get_object_by_index(v[1])
		if item then
			v[2] = math.max(v[2], item:get_count())
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
			local spec = Main.specs:find_by_name("ItemSpec", name)
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
-- @param self Trading.
-- @param player Player.
-- @param merchant Actor.
Trading.start = function(self, player, merchant)
	-- Initialize the trading data.
	self:cancel(player)
	player.trading = {sell = {}, buy = {}, shop = {}, merchant = merchant}
	-- Create the shop list.
	-- TODO: Should depend on the merchant.
	local count = 20
	for i = 1,count do
		local s = Main.specs:find_random("ItemSpec")
		table.insert(player.trading.shop, s.name)
	end
	-- Send the trading start packet.
	Main.messaging:server_event("trading start", player.client, player.trading.shop)
end

--- Sends a deal status update to the player.
-- @param self Trading.
-- @param player Player.
Trading.update = function(self, player)
	Main.messaging:server_event("trading accept", player.client, self:deal(player))
end

return Trading

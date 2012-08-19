Operators.trading = Client

--- Accepts the trade.<br/>
--
-- Context: The trading mode must have been initialized, and the trade must be
-- acceptable.
--
-- @param self Client.
-- @return Table of shop items.
Operators.trading.accept = function(self)
	Game.messaging:client_event("trading accept")
end

--- Gets the list of items selected for buying.<br/>
--
-- Context: The trading mode must have been initialized.
--
-- @param self Client.
-- @return Table of shop items.
Operators.trading.get_buy_items = function(self)
	local items = {}
	for index = 1,5 do
		local shop = self.data.trading.buy[index]
		local item = shop and self.data.trading.shop[shop]
		if item then
			table.insert(items, {
				text = item.spec.name,
				count = item.count or 1,
				icon = item.spec.icon})
		else
			table.insert(items, false)
		end
	end
	return items
end

--- Gets the list of items selected for selling.<br/>
--
-- Context: The trading mode must have been initialized.
--
-- @param self Client.
-- @return Table of shop items.
Operators.trading.get_sell_items = function(self)
	local items = {}
	local object = self.player_object
	if not object then return items end
	for index = 1,5 do
		local inv = self.data.trading.sell[index]
		local item = inv and object.inventory:get_object_by_index(inv)
		local slot = inv and object.inventory:get_slot_by_index(inv)
		if item then
			table.insert(items, {
				text = item.spec.name,
				count = item.count or 1,
				icon = item.spec.icon,
				slot = slot})
		else
			table.insert(items, false)
		end
	end
	return items
end

--- Gets the list of items offered by the shop.<br/>
--
-- Context: The trading mode must have been initialized.
--
-- @param self Client.
-- @return Table of shop items.
Operators.trading.get_shop_items = function(self)
	local items = {}
	for index,item in ipairs(self.data.trading.shop) do
		table.insert(items, {
			text = item.spec.name,
			count = item.count or 1,
			icon = item.spec.icon})
	end
	return items
end

--- Returns true if the trade has been marked as acceptable.<br/>
--
-- Context: The trading mode must have been initialized.
--
-- @param self Client.
-- @return Table of shop items.
Operators.trading.is_acceptable = function(self)
	return self.data.trading.accepted
end

--- Sets the active buy slot index.<br/>
--
-- Context: The trading mode must have been initialized.
--
-- @param self Client.
-- @param index Buy slot index.
Operators.trading.set_buy_index = function(self, index)
	self.data.trading.buy_index = index
end

--- Sets the active buy item index.<br/>
--
-- Context: The trading mode must have been initialized, and the buy index must
-- have been set.
--
-- @param self Client.
-- @param index Buy item index.
Operators.trading.set_buy_item_index = function(self, index)
	self.data.trading.buy_item_index = index
end

--- Marks an item as bought.<br/>
--
-- Context: The trading mode must have been initialized, the buy index must have
-- been set, and the buy item index must have been set.
--
-- @param self Client.
Operators.trading.add_buy = function(self)
	self.data.trading.buy[self.data.trading.buy_index] = self.data.trading.buy_item_index
	self:notify_server()
end

--- Removes an item from the list of bought items.<br/>
--
-- Context: The trading mode must have been initialized, and the buy index must
-- have been set.
--
-- @param self Client.
Operators.trading.remove_buy = function(self)
	self.data.trading.buy[self.data.trading.buy_index] = nil
	self:notify_server()
end

--- Sets the active sell slot index.<br/>
--
-- Context: The trading mode must have been initialized.
--
-- @param self Client.
-- @param index Sell slot index.
Operators.trading.set_sell_index = function(self, index)
	self.data.trading.sell_index = index
end

--- Sets the active sell item index.<br/>
--
-- Context: The trading mode must have been initialized, and the sell index must
-- have been set.
--
-- @param self Client.
-- @param index Sell item index.
Operators.trading.set_sell_item_index = function(self, index)
	self.data.trading.sell_item_index = index
end

--- Marks an item as sold.<br/>
--
-- Context: The trading mode must have been initialized, the sell index must
-- have been set, and the sell item index must have been set.
--
-- @param self Client.
Operators.trading.add_sell = function(self)
	-- Make sure that not already sold.
	local item = self.data.trading.sell_item_index
	for index = 1,5 do
		if self.data.trading.sell[index] == item then return end
	end
	-- Mark as sold.
	self.data.trading.sell[self.data.trading.sell_index] = item
	self:notify_server()
end

--- Removes an item from the list of sold items.<br/>
--
-- Context: The trading mode must have been initialized, and the sell index must
-- have been set.
--
-- @param self Client.
Operators.trading.remove_sell = function(self)
	self.data.trading.sell[self.data.trading.sell_index] = nil
	self:notify_server()
end

--- Sends a trade update message to the server.
--
-- Context: The trading mode must have been initialized.
--
-- @param self Client.
Operators.trading.notify_server = function(self)
	-- Collect bought items.
	local buy = {}
	for k,index in pairs(self.data.trading.buy) do
		local item = self.data.trading.shop[index]
		if item then
			table.insert(buy, {index, item.count})
		else
			self.data.trading.buy[k] = nil
		end
	end
	-- Collect sold items.
	local sell = {}
	local object = self.player_object
	for k,index in pairs(self.data.trading.sell) do
		local item = object.inventory:get_object_by_index(index)
		if item then
			table.insert(sell, {index, item.count})
		else
			self.data.trading.buy[k] = nil
		end
	end
	-- Notify the server.
	Game.messaging:client_event("update trading", buy, sell)
end

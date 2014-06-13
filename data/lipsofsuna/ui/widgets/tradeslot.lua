local Class = require("system/class")
local UiInvItem = require("ui/widgets/invitem")

local UiTradeSlot = Class("Uitradeslot", UiInvItem)

UiTradeSlot.new = function(clss, item, index, buy)
	local self = UiInvItem.new(clss, nil, item, index, item and item.slot)
	self.buy = buy
	if item then
		self.hint = "$A: Remove item\n$$B\n$$U\n$$D"
	else
		self.hint = "$A: Choose item\n$$B\n$$U\n$$D"
	end
	return self
end

UiTradeSlot.apply = function(self)
	if self.item then
		-- Remove the item.
		if self.buy then
			Operators.trading:set_buy_index(self.index)
			Operators.trading:remove_buy()
		else
			Operators.trading:set_sell_index(self.index)
			Operators.trading:remove_sell()
		end
		Ui:restart_state()
	elseif self.buy then
		-- Select an item to be bought.
		Operators.trading:set_buy_index(self.index)
		Ui:push_state("trading/buy")
	else
		-- Select an item to be sold.
		Operators.trading:set_sell_index(self.index)
		Ui:push_state("trading/sell")
	end
	Client.effects:play_global("uitransition1")
end

return UiTradeSlot

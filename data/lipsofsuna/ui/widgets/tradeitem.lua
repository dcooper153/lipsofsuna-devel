local Class = require("system/class")
local UiInvItem = require("ui/widgets/invitem")

local UiTradeItem = Class("Uitradeitem", UiInvItem)

UiTradeItem.new = function(clss, item, index, buy)
	local self = UiInvItem.new(clss, nil, item, index, item and item.slot)
	self.buy = buy
	if item then
		self.hint = "$A: Remove item\n$$B\n$$U\n$$D"
	else
		self.hint = "$A: Choose item\n$$B\n$$U\n$$D"
	end
	return self
end

UiTradeItem.apply = function(self)
	if self.buy then
		Operators.trading:set_buy_item_index(self.index)
		Operators.trading:add_buy()
	else
		Operators.trading:set_sell_item_index(self.index)
		Operators.trading:add_sell()
	end
	Ui:pop_state()
	Main.effect_manager:play_global("uitransition1")
end

return UiTradeItem

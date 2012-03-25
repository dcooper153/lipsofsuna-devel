require(Mod.path .. "invitem")

Widgets.Uitradeslot = Class(Widgets.Uiinvitem)
Widgets.Uitradeslot.class_name = "Widgets.Uitradeslot"

Widgets.Uitradeslot.new = function(clss, item, index, slot, buy)
	local self = Widgets.Uiinvitem.new(clss, nil, item, index, slot)
	self.buy = buy
	if item then
		self.hint = "$A: Remove item\n$$B\n$$U\n$$D"
	else
		self.hint = "$A: Choose item\n$$B\n$$U\n$$D"
	end
	return self
end

Widgets.Uitradeslot.apply = function(self)
	if self.item then
		-- Remove the item.
		if self.buy then
			Client.data.trading.buy[self.index] = nil
		else
			Client.data.trading.sell[self.index] = nil
		end
		Client:update_trade()
		Ui:restart_state()
	elseif self.buy then
		-- Select an item to be bought.
		Client.data.trading.index = self.index
		Ui:push_state("trading/buy")
	else
		-- Select an item to be sold.
		Client.data.trading.index = self.index
		Ui:push_state("trading/sell")
	end
end

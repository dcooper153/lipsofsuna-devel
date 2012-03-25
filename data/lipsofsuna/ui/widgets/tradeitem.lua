require(Mod.path .. "invitem")

Widgets.Uitradeitem = Class(Widgets.Uiinvitem)
Widgets.Uitradeitem.class_name = "Widgets.Uitradeitem"

Widgets.Uitradeitem.new = function(clss, item, index, slot, buy)
	local self = Widgets.Uiinvitem.new(clss, nil, item, index, slot)
	self.buy = buy
	if item then
		self.hint = "$A: Remove item\n$$B\n$$U\n$$D"
	else
		self.hint = "$A: Choose item\n$$B\n$$U\n$$D"
	end
	return self
end

Widgets.Uitradeitem.apply = function(self)
	local index = Client.data.trading.index
	if self.buy then
		Client.data.trading.buy[index] = self.index
	else
		Client.data.trading.sell[index] = self.index
	end
	Client:update_trade()
	Ui:pop_state()
end

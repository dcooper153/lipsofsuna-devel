require(Mod.path .. "invitem")

Widgets.Uitradeitem = Class(Widgets.Uiinvitem)
Widgets.Uitradeitem.class_name = "Widgets.Uitradeitem"

Widgets.Uitradeitem.new = function(clss, item, index, buy)
	local self = Widgets.Uiinvitem.new(clss, nil, item, index, item and item.slot)
	self.buy = buy
	if item then
		self.hint = "$A: Remove item\n$$B\n$$U\n$$D"
	else
		self.hint = "$A: Choose item\n$$B\n$$U\n$$D"
	end
	return self
end

Widgets.Uitradeitem.apply = function(self)
	if self.buy then
		Operators.trading:set_buy_item_index(self.index)
		Operators.trading:add_buy()
	else
		Operators.trading:set_sell_item_index(self.index)
		Operators.trading:add_sell()
	end
	Ui:pop_state()
	Effect:play_global("uitransition1")
end

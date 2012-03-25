Protocol:add_handler{type = "TRADING_ACCEPT", func = function(event)
	-- Read deal status.
	local ok,deal = event.packet:read("bool")
	if not ok then return end
	-- Change the shopkeeper's approval.
	Client.data.trading.accepted = deal
	-- Update the user interface.
	if Ui.state == "trading" then
		Ui:restart_state()
	end
end}

Protocol:add_handler{type = "TRADING_CLOSE", func = function(event)
	-- Update the user interface.
	if Ui.state == "trading" then
		Ui:pop_state()
	end
end}

Protocol:add_handler{type = "TRADING_START", func = function(event)
	-- Read the shop list length.
	local ok,count = event.packet:read("uint8")
	if not ok then return end
	-- Clear the shop.
	Client.data.trading = {buy = {}, sell = {}, shop = {}}
	-- Read the shop list.
	for i=1,count do
		local ok,name = event.packet:resume("string")
		if not ok then break end
		local spec = Itemspec:find{name = name}
		local item = {spec = spec, count = 1}
		Client.data.trading.shop[i] = item
	end
	-- Update the user interface.
	Ui.state = "trading"
end}

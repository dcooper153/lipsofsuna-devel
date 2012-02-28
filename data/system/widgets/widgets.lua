Widgets.find_handler_widget = function(clss, handler, args)
	local w = clss:find_widget(args)
	while w do
		if w[handler] then return w end
		w = w.parent
	end
end

Widgets.handle_event = function(clss, args)
	if Program.cursor_grabbed then return end
	local generic = {keypress = true, keyrelease = true, mouserelease = true}
	if args.type == "mousepress" then
		-- Buttons.
		local w = clss:find_handler_widget("pressed")
		if clss:handle_popups(w) then return end
		if w then return w:pressed(args) end
	elseif args.type == "mousescroll" then
		-- Scroll wheel.
		local w = clss:find_handler_widget("scrolled")
		if clss:handle_popups(w) then return end
		if w then return w:scrolled(args) end
	elseif args.type == "mousemotion" then
		-- Motion.
		local w = clss:find_handler_widget("mousemotion")
		if w then return w:mousemotion(args) end
	elseif generic[args.type] then
		-- Other events.
		local w = clss:find_handler_widget("event")
		if w and w.event then
			w:event(args)
			return true
		end
	end
end

Widgets.handle_popups = function(clss, w)
	if not clss.popup then return end
	local p = w
	while p do
		if p == clss.popup then return end
		p = p.parent
	end
	clss.popup.visible = false
	clss.popup = nil
	return true
end

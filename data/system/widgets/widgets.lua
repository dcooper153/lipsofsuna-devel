local oldpopup = Widget.popup

Widget.popup = function(self, args)
	if Widgets.popup then
		Widgets.popup.visible = false
	end
	Widgets.popup = self
	return oldpopup(self, args)
end

Widgets.find_handler_widget = function(clss, handler, args)
	local w = clss:find_widget(args)
	while w do
		if w[handler] then return w end
		w = w.parent
	end
end

Widgets.handle_event = function(clss, args)
	local generic = {keypress = true, keyrelease = true, mousemotion = true, mouserelease = true}
	if args.type == "mousepress" then
		-- Mouse button pressed and scroll wheel.
		if args.button ~= 4 and args.button ~= 5 then
			local w = clss:find_handler_widget("pressed")
			if clss:handle_popups(w) then return end
			if w then return w:pressed(args) end
		else
			local w = clss:find_handler_widget("scrolled")
			if clss:handle_popups(w) then return end
			if w then return w:scrolled(args) end
		end
	elseif generic[args.type] then
		-- Other events.
		local w = clss.focused_widget_prev
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

Widgets.update = function(clss)
	local w = clss.focused_widget
	if clss.focused_widget_prev ~= w then
		if clss.focused_widget_prev then
			clss.focused_widget_prev.focused = false
		end
		if w then
			w.focused = true
		end
		clss.focused_widget_prev = w
	end
end

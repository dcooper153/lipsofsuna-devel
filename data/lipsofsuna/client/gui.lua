Ui = Class()
Ui.class_name = "Ui"
Ui.bubbles = {}
Ui.huds = {}
Ui.states = {}

--- Initializes the user interface state system.
-- @param self Ui class.
Ui.init = function(self)
	self.history = {}
	self.size = Vector()
	self.stack = {}
	self.widgets = {}
	self.window = Widget{floating = true, fullscreen = true}
	self.repeat_timer = 0
	-- Create the back button.
	self.back = Widgets.Button{
		text = "<",
		pressed = function() self:pop_state() end}
	-- Create the help labels.
	self.hint = Widgets.Label()
	self.hint:set_request{width = 200}
	self.label = Widgets.Label{font = "medium"}
	self.label:set_request{width = 200}
	-- Create the scrollbar.
	self.scrollbar = Widgets.Scrollbar{
		offset = Vector(32, 0),
		changed = function(w, p) self.scroll_offset = p end,
		grabbed = function(w, v) self.scroll_active = v end}
	self.scrollbar.step = 50
	-- Initialize the cursor.
	self.cursor = Widgets.Cursor(Iconspec:find{name = "cursor1"})
	self.pointer_grab = Options.inst.grab_cursor
end

--- Adds a heads over display widget to the user interface.
-- @param self Ui class.
-- @param args Arguments.<ul>
--   <li>active: Activity control function.</li>
--   <li>id: Optional ID for identification.</li>
--   <li>init: Initializer function.</li></ul>
Ui.add_hud = function(self, args)
	-- Try to override an existing HUD.
	local hud,index = self:get_hud(args.id)
	if hud then
		self.huds[index] = args
		return
	end
	-- Add a new HUD.
	table.insert(self.huds, args)
end

--- Adds a speech bubble widget.
-- @param self Ui class.
-- @param widget Widget.
Ui.add_speech_bubble = function(self, widget)
	-- Add to the canvas.
	if self.background then
		self.background:add_child(widget)
	else
		self.window:add_child(widget)
	end
	-- Add to the list.
	table.insert(self.bubbles, widget)
end

--- Removes a speech bubble widget.
-- @param self Ui class.
-- @param widget Widget.
Ui.remove_speech_bubble = function(self, widget)
	-- Remove from the canvas.
	widget:detach()
	-- Remove from the list.
	for k,v in pairs(self.bubbles) do
		if v == widget then
			table.remove(self.bubbles, k)
			return
		end
	end
end

--- Adds a state to the user interface.
-- @param self Ui class.
-- @param args Arguments.<ul>
--   <li>background: Background creation function.</li>
--   <li>grab: Grab disable function.</li>
--   <li>hint: Controls hint text shown to the user.</li>
--   <li>init: Initializer function.</li>
--   <li>input: Input handler function.</li>
--   <li>label: State name shown to the user.</li>
--   <li>root: Root state name.</li>
--   <li>state: State name.</li>
--   <li>update: Update function.</li></ul>
-- @return State.
Ui.add_state = function(self, args)
	-- Get or create the state.
	local state = self.states[args.state]
	if not state then
		state = {
			background = function() end,
			grab = function() return true end,
			ids = {},
			init = {},
			input = {},
			update = {},
			widgets = {}}
		self.states[args.state] = state
	end
	-- Set the background.
	if args.background then
		state.background = args.background
	end
	-- Set the pointer grab mode.
	if args.grab ~= nil then
		state.grab = args.grab
	end
	-- Set the hint text.
	if args.hint then
		state.hint = args.hint
	end
	-- Add the initializer.
	if args.init then
		table.insert(state.init, args.init)
	end
	-- Add the input handler.
	if args.input then
		table.insert(state.input, args.input)
	end
	-- Set the label text.
	if args.label then
		state.label = args.label
	end
	-- Set the root state.
	if args.root then
		state.root = args.root
	end
	-- Add the update function.
	if args.update then
		table.insert(state.update, args.update)
	end
	return state
end

--- Adds a widget to a state.
-- @param self Ui class.
-- @param args Arguments.<ul>
--   <li>help: Help message string for the widget.</li>
--   <li>hint: Controls hint message string for the widget.</li>
--   <li>id: Optional widget ID for later identification.</li>
--   <li>input: Input handler function.</li>
--   <li>state: State name.</li>
--   <li>update: Update function for the widget.</li>
--   <li>widget: Widget creation function.</li></ul>
Ui.add_widget = function(self, args)
	-- Create the widget specification.
	local spec = {init = args.widget}
	if args.input then
		spec.input = args.input
		args.input = nil
	end
	if args.update then
		spec.update = args.update
		args.update = nil
	end
	if args.help then
		spec.help = args.help
		args.help = nil
	end
	if args.hint then
		spec.hint = args.hint
		args.hint = nil
	end
	-- Get or create the state.
	local state = self:add_state(args)
	-- Add the widget to the state.
	-- If the widget was given an ID, try to replace an existing widget with the
	-- same ID. If no ID was given or no replacement occurred, append to the list.
	if args.id then
		local index = state.ids[args.id]
		if index then
			state.widgets[index] = spec
			return
		end
		state.ids[args.id] = #state.widgets + 1
	end
	table.insert(state.widgets, spec)
end

--- Scrolls the focused widget to the screen.
-- @param self Ui class.
Ui.autoscroll = function(self)
	-- Get the focused widget.
	local w = self.focused_widget
	if not w then return end
	-- Get the relative screen position.
	local start = self.widgets[1].y
	local height = self.size.y
	-- Scroll up until the top of the widget is visible.
	local top = w.offset.y
	if top < 0 then
		self.scroll_offset = top - start
		return
	end
	-- Scroll down until the bottom of the widget is visible.
	local bottom = top + w.size.y
	if bottom > height then
		self.scroll_offset = bottom - start - height
	end
end

--- Performs a control command in the UI.
-- @param self Ui class.
-- @param cmd Command string.
-- @param press True for start, false for end.
Ui.command = function(self, cmd, press)
	if not press then
		if cmd == "up" then self.repeat_up = nil end
		if cmd == "down" then self.repeat_down = nil end
		return
	end
	if cmd == "back" then
		local widget = self.widgets[self.focused_item]
		if not widget or not widget.apply_back then return self:pop_state() end
		if widget:apply_back() then self:pop_state() end
	elseif cmd == "apply" then
		if self.focused_item then
			local widget = self.widgets[self.focused_item]
			if widget and widget.apply then widget:apply() end
		end
	elseif cmd == "up" then
		self.repeat_timer = 0
		self.repeat_up = true
		if self.focused_item then
			if self.focused_item > 1 then
				self.widgets[self.focused_item].focused = false
				self.focused_item = self.focused_item - 1
				self.widgets[self.focused_item].focused = true
				self:update_help()
				self:autoscroll()
			elseif #self.widgets > 1 then
				self.widgets[self.focused_item].focused = false
				self.focused_item = #self.widgets
				self.widgets[self.focused_item].focused = true
				self:update_help()
				self:autoscroll()
			end
			self.history[self.state] = self.focused_item
		end
	elseif cmd == "down" then
		self.repeat_timer = 0
		self.repeat_down = true
		if self.focused_item then
			if self.focused_item < #self.widgets then
				self.widgets[self.focused_item].focused = false
				self.focused_item = self.focused_item + 1
				self.widgets[self.focused_item].focused = true
				self:update_help()
				self:autoscroll()
			elseif #self.widgets > 1 then
				self.widgets[self.focused_item].focused = false
				self.focused_item = 1
				self.widgets[self.focused_item].focused = true
				self:update_help()
				self:autoscroll()
			end
			self.history[self.state] = self.focused_item
		end
	end
end

--- Disables a heads over display widget.
-- @param self Ui class.
-- @param id Widget ID.
Ui.disable_hud = function(self, args)
	-- Find the HUD.
	local hud = self:get_hud(args.id)
	if not hud then return end
	-- Deinitialize if not done already.
	if not hud.widget then return end
	if hud.free then hud.free(hud.widget) end
	-- Remove from the canvas.
	hud.widget:detach()
	hud.widget = nil
end

--- Enables a heads over display widget.
-- @param self Ui class.
-- @param id Widget ID.
Ui.enable_hud = function(self, id)
	-- Find the HUD.
	local hud = self:get_hud(id)
	if not hud then return end
	-- Initialize if not done already.
	if hud.widget then return end
	hud.widget = hud.init()
	if not hud.widget then return end
	-- Queue a widget repack.
	-- The HUD is below state widgets so we need to detach and reattach all
	-- widgets to get their order right. This is done in the update function
	-- to avoid multiple repacks per frame.
	self.need_repack = true
end

--- Finds a heads over display by ID.
-- @param self Ui class.
-- @param id Widget ID.
-- @return Widget or nil, list index or nil.
Ui.get_hud = function(self, id)
	if not id then return end
	for index,hud in ipairs(self.huds) do
		if hud.id == id then
			return hud,index
		end
	end
end

--- Finds a widget from the current state by ID.
-- @param self Ui class.
-- @param id Widget ID.
-- @return Widget or nil.
Ui.get_widget = function(self, id)
	if not id then return end
	for _,widget in pairs(self.widgets) do
		if widget.id == id then
			return widget
		end
	end
end

--- Handles an input event.
-- @param self Ui class.
-- @param args Event arguments.
-- @return True if the caller should handle the event.
Ui.handle_event = function(self, args)
	-- Find the state.
	local state_ = self.states[self.state]
	if not state_ then return true end
	-- Check the mouse event handling mode.
	local mouse_mode = not self.pointer_grab
	local mouse_event = false
	if args.type == "mousepress" or args.type == "mouserelease" or
	   args.type == "mousescroll" or args.type == "mousemotion" then
		mouse_event = true
	end
	-- Call the event handler of the active widget.
	if self.focused_item and (not mouse_mode or not mouse_event) then
		local widget = self.widgets[self.focused_item]
		if widget and not widget:handle_event(args) then return end
	end
	-- Call the event handler functions of the state.
	for k,v in pairs(state_.input) do
		if not v(args) then return end
	end
	-- Tell the caller to handle unhandled events.
	if not mouse_mode or not mouse_event then
		return true
	end
	-- Hijack input when the scrollbar is grabbed.
	if self.scroll_active then
		self.scrollbar:handle_event(args)
		return
	end
	-- Call the event handler of the widget under the cursor.
	local widget = Widgets:find_handler_widget("handle_event")
	if widget and not widget:handle_event(args) then
		return
	end
	-- Scroll with remaining mouse wheel events.
	if args.type == "mousescroll" then
		self:scroll(args.rel > 0 and "up" or "down")
	end
end

--- Pushes a new state on top of the state stack and shows it.
-- @param self Ui class.
-- @param state State name.
-- @param focus Focused widget index, nil for first.
Ui.push_state = function(self, state, focus)
	table.insert(self.stack, state)
	self:show_state(state, focus)
end

--- Pops the topmost state in the stack and shows the state below it.
-- @param self Ui class.
Ui.pop_state = function(self)
	table.remove(self.stack, #self.stack)
	self:show_state(self.state)
end

--- Repaints all the widgets of the state.
-- @param self Ui class.
Ui.repaint_state = function(self)
	for k,v in pairs(self.widgets) do
		v.need_repaint = true
	end
end

--- Recreates and shows the current state.
-- @param self Ui class.
Ui.restart_state = function(self)
	local s = self.state
	local f = self.focused_item
	-- Remove and recreate the state.
	self:pop_state()
	self:push_state(s, f)
end

--- Scrolls the screen.
-- @param self Ui class.
Ui.scroll = function(self, dir)
	if not self.scrollbar.visible then return end
	local range = self.__scroll_range
	if not range then return end
	if dir == "up" then
		self.scroll_offset = math.max(range[2] - 50, 0)
	elseif dir == "down" then
		self.scroll_offset = math.min(range[2] + 50, range[1] - range[3])
	end
end

--- Shows a state by name.
-- @param self Ui class.
-- @param state State name.
-- @param focus Focused widget index, nil for first.
Ui.show_state = function(self, state, focus)
	local y = 0
	local root = self.window
	local add = function(widget)
		table.insert(self.widgets, widget)
		y = y + widget.size.y
	end
	-- Detach the widgets.
	self:show_state_detach()
	self.widgets = {}
	-- Remove the old background.
	if self.background then
		self.background:detach()
		self.background = nil
	end
	-- Find the state.
	if not state then return end
	local state_ = self.states[state]
	if not state_ then return end
	-- Update the root state name.
	if state_.root then
		self.root = state_.root
	end
	-- Create the background.
	if state_.background then
		local ret,widget = xpcall(state_.background, function(err)
			print(debug.traceback("ERROR: " .. err))
		end)
		if widget then
			widget:set_request{width = self.window.width, height = self.window.height}
			self.window:add_child(widget)
			self.background = widget
			root = widget
		end
	end
	-- Call the initializers.
	for id,func in pairs(state_.init) do
		local ret,widgets = xpcall(func, function(err)
			print(debug.traceback("ERROR: " .. err))
		end)
		if ret and widgets then
			for k,widget in ipairs(widgets) do add(widget) end
		end
	end
	-- Create the widgets.
	for id,spec in ipairs(state_.widgets) do
		local ret,widget = xpcall(spec.init, function(err)
			print(debug.traceback("ERROR: " .. err))
		end)
		if ret and widget then
			if widget.class_name then
				if spec.help then widget.help = spec.help end
				if spec.hint then widget.hint = spec.hint end
				if spec.input then widget.handle_input = spec.input end
				if spec.update then widget.update = spec.update end
				if not widget.id then widget.id = id end
				add(widget)
			else
				for k,v in ipairs(widget) do add(v) end
			end
		end
	end
	-- Set the state name text.
	local mode = Program.video_mode
	if state_.label then
		self.label.text = state_.label
		self.hint.offset = Vector(mode[1] - 200, 30)
	else
		self.label.text = ""
		self.hint.offset = Vector(mode[1] - 200, 0)
	end
	-- Toggle HUD widgets.
	for index,hud in pairs(self.huds) do
		if hud.active and hud.active() then
			if not hud.widget then
				hud.widget = hud.init()
			end
		else
			if hud.widget then
				if hud.free then hud.free(hud.widget) end
				hud.widget:detach()
				hud.widget = nil
			end
		end
	end
	-- Attach the widgets.
	self:show_state_attach()
	-- Focus the first or previously focused widget.
	local f = math.min(focus or self.history[state] or 1, #self.widgets)
	if self.widgets[f] then
		self.focused_item = f
		self.widgets[f].focused = true
	else
		self.focused_item = nil
	end
	self.history[state] = self.focused_item
	-- Position the widgets.
	self.scroll_offset = 0
	self.need_autoscroll = true
	-- Rebuild the help text.
	self:update_help()
	-- Update the grab state.
	self.pointer_grab = self.pointer_grab
end

--- Internal function that adds all widgets to the window in the right order.
-- @param self Ui class.
Ui.show_state_attach = function(self)
	local root = self.background or self.window
	-- Attach the speech bubble widgets.
	for k,v in pairs(self.bubbles) do
		root:add_child(v)
	end
	-- Attach the HUD widgets.
	for k,v in pairs(self.huds) do
		if v.widget then root:add_child(v.widget) end
	end
	-- Attach the state widgets.
	for k,v in pairs(self.widgets) do
		root:add_child(v)
	end
	-- Attach the navigation widgets.
	if self.widgets[1] then
		root:add_child(self.back)
	end
	if Client.options.help_messages then
		root:add_child(self.hint)
		if self.label.text ~= "" then
			root:add_child(self.label)
		end
	end
	if self.widgets[1] then
		root:add_child(self.scrollbar)
	end
	-- Mark repacking as done.
	self.need_repack = nil
end

--- Internal function that removes all widgets from the window.
-- @param self Ui class.
Ui.show_state_detach = function(self)
	-- Detach the speech bubble widgets.
	for k,v in pairs(self.bubbles) do
		v:detach()
	end
	-- Detach the HUD widgets.
	for k,v in pairs(self.huds) do
		if v.widget then v.widget:detach() end
	end
	-- Detach the state widgets.
	for k,v in pairs(self.widgets) do
		v:detach()
	end
	-- Detach the navigation widgets.
	self.back:detach()
	self.hint:detach()
	self.label:detach()
	self.scrollbar:detach()
end

--- Updates the user interface system.
-- @param self Ui class.
-- @param secs Seconds since the last update.
Ui.update = function(self, secs)
	-- Update the window size.
	local mode = Program.video_mode
	if mode[1] ~= self.size.x or mode[2] ~= self.size.y then
		self.size.x = mode[1]
		self.size.y = mode[2]
		self.window:set_request{width = mode[1], height = mode[2]}
		if self.background then
			self.background:set_request{width = mode[1], height = mode[2]}
		end
		self.label.offset = Vector(mode[1] - 200, 0)
		if self.label.text ~= "" then
			self.hint.offset = Vector(mode[1] - 200, 30)
		else
			self.hint.offset = Vector(mode[1] - 200, 0)
		end
		self.need_relayout = true
	end
	-- Update the cursor.
	if not self.pointer_grab then
		self.cursor:update()
	end
	-- Call the update functions of the state.
	local s = self.states[self.state]
	if s then
		for k,v in pairs(s.update) do
			v(secs)
		end
	end
	-- Call the update functions of the widgets.
	for k,v in pairs(self.widgets) do
		if v.update then v:update(secs) end
	end
	-- Call the update functions of the HUD.
	for k,v in pairs(self.huds) do
		if v.widget and v.widget.update then v.widget:update(secs) end
	end
	-- Update the layout.
	if self.need_relayout then
		self.need_relayout = nil
		self.scroll_offset = self.scroll_offset or 0
	end
	-- Autoscroll after state changes.
	if self.need_autoscroll then
		self.need_autoscroll = nil
		self:autoscroll()
	end
	-- Repack if a HUD was enabled.
	if self.need_repack then
		self:show_state_detach()
		self:show_state_attach()
	end
	-- Implement key repeat for browsing.
	self.repeat_timer = self.repeat_timer + secs
	if self.repeat_timer >= 0.15 then
		self.repeat_timer = 0
		local action1 = Binding.dict_name["menu up"]
		if action1.key1 and Binding.dict_press[action1.key1] and self.repeat_up then
			self:command("up", true)
		end
		local action2 = Binding.dict_name["menu down"]
		if action2.key1 and Binding.dict_press[action2.key1] and self.repeat_down then
			self:command("down", true)
		end
	end
	-- Update mouse focus.
	if not self.pointer_grab then
		local focus = Widgets.widget_under_cursor
		if focus then
			-- Focus a UI widget.
			local found = false
			for k,v in pairs(self.widgets) do
				if v == focus then
					if self.focused_item ~= k then
						self.widgets[self.focused_item].focused = false
						self.focused_item = k
						self.widgets[self.focused_item].focused = true
						self:update_help()
						self:autoscroll()
					end
					if self.prev_custom_focus then
						self.prev_custom_focus.focused = false
						self.prev_custom_focus = nil
					end
					found = true
					break
				end
			end
			-- Focus a custom widget.
			if not found then
				if self.prev_custom_focus then
					self.prev_custom_focus.focused = false
				end
				self.prev_custom_focus = focus
				focus.focused = true
			end
		end
	end
end

--- Updates the help text of the state.
-- @param self Ui class.
Ui.update_help = function(self)
	-- Get the hint format.
	local hint
	local widget = self.focused_widget
	if not widget then
		local state = self.states[self.state]
		if state and state.hint then
			hint = state.hint
		else
			hint = "$$B"
		end
	else
		hint = widget.hint or "$$A\n$$B\n$$U\n$$D"
	end
	-- Replace special format sequences.
	hint = string.gsub(hint, "$$A", "$A: Activate")
	hint = string.gsub(hint, "$$B", "$B: Close menu")
	hint = string.gsub(hint, "$$U", "$U: Previous item")
	hint = string.gsub(hint, "$$D", "$D: Next item")
	hint = string.gsub(hint, "$A", Binding:get_control_name("menu apply") or "[---]")
	hint = string.gsub(hint, "$B", Binding:get_control_name("menu back") or "[---]")
	hint = string.gsub(hint, "$D", Binding:get_control_name("menu down") or "[---]")
	hint = string.gsub(hint, "$U", Binding:get_control_name("menu up") or "[---]")
	-- Get the detailed help string.
	if widget then
		help = widget.help
	end
	if help then
		hint = hint .. "\n\n" .. help
	end
	-- Show the formatted hint text.
	self.hint.text = hint
end

Ui:add_class_getters{
	focused_widget = function(self)
		if not self.focused_item then return end
		return self.widgets[self.focused_item]
	end,
	pointer_grab = function(self)
		return rawget(self, "__pointer_grab")
	end,
	scroll_offset = function(self)
		return rawget(self, "__scroll_offset")
	end,
	state = function(self)
		local s = self.stack[#self.stack]
		return s or self.root or "play"
	end}

Ui:add_class_setters{
	pointer_grab = function(self, v)
		-- Set cursor visibility.
		rawset(self, "__pointer_grab", v)
		self.cursor.floating = not v
		-- Set pointer grabbing.
		-- If the state indicates that the cursor does not need to be grabbed,
		-- we just hide it so that it can leave the window in windowed mode.
		local state_ = self.states[self.state]
		if v then
			if state_ and state_.grab() then
				Program.cursor_grabbed = true
			else
				Program.cursor_grabbed = false
			end
		else
			Program.cursor_grabbed = false
		end
	end,
	state = function(self, v)
		self.stack = {v}
		self:show_state(v)
	end,
	scroll_offset = function(self, v)
		-- Decide if a scrollbar is needed.
		local h = 0
		for k,widget in ipairs(self.widgets) do
			h = h + widget.size.y
		end
		local x = (h < self.size.y) and 32 or 53
		-- Update system widget positions.
		if h < self.size.y then
			self.scrollbar.visible = false
			self.back:set_request{width = 32, height = h}
		else
			self.scrollbar:set_range(h, v, self.size.y)
			self.scrollbar:set_request{height = self.size.y}
			self.scrollbar.visible = true
			self.back:set_request{width = 32, height = self.size.y}
		end
		-- Update state widget positions.
		local y = 0
		local sh = self.size.y
		for k,widget in ipairs(self.widgets) do
			local wy = y - v
			local wh = widget.size.y
			widget.offset = Vector(x, wy)
			widget.visible = (wy > -wh and wy < sh)
			y = y + wh
		end
		-- Store the scrolling state.
		rawset(self, "__scroll_offset", v)
		rawset(self, "__scroll_range", {h, v, self.size.y})
	end}

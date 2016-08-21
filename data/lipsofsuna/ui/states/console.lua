local UiEntry = require("ui/widgets/entry")
local ConfigFile = require("system/config-file")

Ui:add_state{
	state = "console",
	label = "Lua Console"}

Ui:add_widget{
	state = "console",
	widget = function()
		local widget = UiEntry("Lua Console")
		widget.input_mode = true
		widget.hint = "[RETURN]: Execute\n[ESCAPE]: Close\n[ALT-P/N]Previous/next"
		widget:history_enable(true)
		local console_history_file = ConfigFile{name = "console_history.cfg"}
		local k = 1
		local v
		repeat
			local v = console_history_file:get("" .. k)
			if v ~= nil then
				widget.history[k] = v
				k = k + 1
			end
		until v == nil
		widget.history_index = k
		widget.accepted = function(w)
			-- Perform the command.
			local func,err = loadstring(w.value)
			local okay,res
			if err then
				Client:append_log(err)
				okay = false
			else
				okay,res = xpcall(func, function(err)
					print(debug.traceback("ERROR: " .. err))
				end)
				print(okay,res)
			end
			-- Close the console unless the command did already.
			--if Ui:get_state() == "console" then Ui:pop_state() end
			--Main.effect_manager:play_global("uitransition1")
			if okay then
				w:history_add(w.value)
				w.value = ""
				w.cursor_pos = 1
			end
			w.input_mode = true
			w:set_focused(true)
		end
		widget.canceled = function(w)
			Ui:pop_state()
			Main.effect_manager:play_global("uitransition1")
			local console_history_file = ConfigFile{name = "console_history.cfg"}
			for k,v in ipairs(w.history) do
				console_history_file:set("" .. k, v)
			end
			console_history_file:save()
		end
		return widget
	end}

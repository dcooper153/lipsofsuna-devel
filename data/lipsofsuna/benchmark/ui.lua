Ui:add_state{
	state = "benchmark",
	root = "benchmark",
	exit_root = function()
		Main.benchmark:close()
		Main.benchmark = nil
	end,
	init = function()
		Client.music:switch_track("game")
	end,
	update = function(secs)
		local w = Ui:get_widget("stats")
		if w then
			w.timer = (w.timer or 0) + secs
			if w.timer > 1 then
				w.timer = 0
				Operators.stats:update_client_stats()
				w.text = Operators.stats:get_client_stats_text()
				w.need_reshape = true
				w.need_repaint = true
				Ui:queue_relayout()
			end
		end
	end}

Ui:add_widget{
	state = "benchmark",
	id = "stats",
	widget = function()
		Operators.stats:update_client_stats()
		return Widgets.Uilabel(Operators.stats:get_client_stats_text())
	end}

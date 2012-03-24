Ui:add_state{
	state = "book",
	label = "Book",
	init = function()
		local split = function(str, pat)
			local t = {}
			local fpat = "(.-)" .. pat
			local last_end = 1
			local s,e,cap = str:find(fpat, 1)
			while s do
				if s ~= 1 or cap ~= "" then
					table.insert(t,cap)
				end
				last_end = e+1
				s,e,cap = str:find(fpat, last_end)
			end
			if last_end <= #str then
				cap = str:sub(last_end)
				table.insert(t, cap)
			end
			return t
		end
		-- Create the title widget.
		local widgets = {Widgets.Uilabel(Client.data.book.title)}
		-- Create the page widgets.
		local pages = split(Client.data.book.text, "\n\n\n")
		for k,v in ipairs(pages) do
			table.insert(widgets, Widgets.Uilabel(v))
		end
		return widgets
	end}

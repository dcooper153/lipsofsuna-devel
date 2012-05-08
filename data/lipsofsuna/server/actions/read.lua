Actionspec:extend{
	name = "read",
	func = function(item, user)
		-- Trigger book dialogs.
		if not item.dialog then
			local dialog = Dialog{object = item, user = user}
			if dialog then
				item.dialog = dialog
				item.dialog:execute()
			end
		end
		-- Send the book text.
		user:send{packet = Packet(packets.BOOK,
			"string", item.spec.name,
			"string", item.spec.book_text)}
	end}

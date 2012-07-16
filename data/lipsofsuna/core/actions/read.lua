Actionspec{
	name = "read",
	label = "Read",
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
		Game.messaging:server_event("read", user.client, item.spec.name, item.spec.book_text)
	end}

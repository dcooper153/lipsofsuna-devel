Actionspec{
	name = "read",
	label = "Read",
	func = function(item, user)
		-- Trigger book dialogs.
		if not Main.dialogs then return end
		Main.dialogs:execute(item, user)
		Main.dialogs:cancel(item)
		-- Send the book text.
		Main.messaging:server_event("read", user.client, item.spec.name, item.spec.book_text)
	end}

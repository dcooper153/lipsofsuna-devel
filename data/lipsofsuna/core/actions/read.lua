Actionspec{
	name = "read",
	label = "Read",
	func = function(item, user)
		-- Trigger book dialogs.
		Server.dialogs:execute(item, user)
		Server.dialogs:cancel(item)
		-- Send the book text.
		Game.messaging:server_event("read", user.client, item.spec.name, item.spec.book_text)
	end}

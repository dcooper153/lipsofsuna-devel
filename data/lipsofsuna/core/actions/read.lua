Actionspec{
	name = "read",
	label = "Read",
	start = function(action, item)
		-- Trigger book dialogs.
		if not Main.dialogs then return end
		Main.dialogs:execute(item, action.object)
		Main.dialogs:cancel(item)
		-- Send the book text.
		Main.messaging:server_event("read", action.object.client, item.spec.name, item.spec.book_text)
	end}

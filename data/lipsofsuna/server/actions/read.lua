Actionspec:extend{
	name = "read",
	func = function(item, user)
		user:send{packet = Packet(packets.BOOK,
			"string", item.spec.name,
			"string", item.spec.book_text)}
	end}

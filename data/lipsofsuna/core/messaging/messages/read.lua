-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "read",
	server_to_client_encode = function(self, title, text)
		return {"string", title, "string", text}
	end,
	server_to_client_decode = function(self, packet)
		local ok,title,text = packet:read("string", "string")
		if not ok then return end
		return {title, text}
	end,
	server_to_client_handle = function(self, title, text)
		Client.data.book.title = title
		Client.data.book.text = text
		Ui:set_state("book")
	end}

Message{
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
		Ui.state = "book"
	end}

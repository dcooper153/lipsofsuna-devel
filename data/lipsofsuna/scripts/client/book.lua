Book = Class()

--- Initializes the book UI.
-- @param clss Book class.
Book.init = function(clss)
	clss.dialog = Group{cols = 1, style = "window"}
	clss.title = Button{style = "label"}
	clss.text = Button{style = "label", width = 100}
	clss.text:set_request{width = 300}
	clss.button = Button{text = "Close", pressed = function() Book.dialog.floating = false end}
	clss.dialog:append_row(clss.title)
	clss.dialog:append_row(clss.text)
	clss.dialog:append_row(clss.button)
end

--- Shows the book UI.
-- @param clss Book class.
-- @param args Arguments.<ul>
--   <li>text: Content string.</li>
--   <li>title: Title string.</li></ul>
Book.show = function(clss, args)
	clss.title.text = "Book: " .. args.title
	clss.text.text = args.text
	clss.dialog.floating = true
end

Book:init()

Protocol:add_handler{type = "BOOK", func = function(event)
	local ok,title,text = event.packet:read("string", "string")
	if ok then
		Book:show{title = title, text = text}
	end
end}

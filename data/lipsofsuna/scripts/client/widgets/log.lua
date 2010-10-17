Widgets.Log = Class(Group)

Widgets.Log.new = function(clss, args)
	local self = Group.new(clss, args)
	self.button = Button{style = "label"}
	self.lines = {"", "", "", "", ""}
	self.cols = 1
	self.rows = 1
	self:append_row(self.button)
	self:set_expand{row = 1}
	return self
end

Widgets.Log.append = function(self, args)
	local l = self.lines
	-- Scroll up lines.
	for i = 2,#l do
		l[i - 1] = l[i]
	end
	-- Append the new line.
	l[#l] = args.text
	-- Rebuild the text.
	local t = l[1]
	for i = 2,#l do
		t = t .. "\n" .. l[i]
	end
	self.button.text = t
end

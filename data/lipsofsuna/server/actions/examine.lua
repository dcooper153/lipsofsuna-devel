Actionspec:extend{
	name = "examine",
	func = function(self, user)
		if self.dialog then return end
		local dialog = Dialog{object = self, user = user}
		if not dialog then return end
		self.dialog = dialog
		self.dialog:execute()
	end}

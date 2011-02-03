require "client/widgets/text"

Widgets.QuestInfo = Class(Widgets.Text)

Widgets.QuestInfo.setter = function(self, key, value)
	if key == "quest" then
		self.text = {
			{value.name, "medium"},
			{value.status == "completed" and "Status: Completed" or "Status: Active"},
			{value.text}}
		self:build()
	else
		Widgets.Text.setter(self, key, value)
	end
end

Widgets.QuestInfo.new = function(clss, args)
	local self = Widgets.Text.new(clss, args)
	self.text = {{"No quests"}}
	return self
end

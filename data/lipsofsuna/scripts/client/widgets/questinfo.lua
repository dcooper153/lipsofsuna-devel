Widgets.QuestInfo = Class(Widget)

Widgets.QuestInfo.setter = function(self, key, value)
	if key == "quest" then
		self.label_name.text = value.name
		self.label_desc.text = value.text
		if value.status == "completed" then
			self.label_status.text = "Status: Completed"
		else
			self.label_status.text = "Status: Active"
		end
	else
		Widget.setter(self, key, value)
	end
end

Widgets.QuestInfo.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.cols = 1
	self.rows = 4
	self.label_name = Widgets.Label{font = "medium"}
	self.label_status = Widgets.Label{text = "No quests"}
	self.label_desc = Widgets.Label()
	self.label_desc:set_request{width = 300}
	self:set_expand{row = 4}
	self:set_child{col = 1, row = 1, widget = self.label_name}
	self:set_child{col = 1, row = 2, widget = self.label_status}
	self:set_child{col = 1, row = 3, widget = self.label_desc}
	return self
end

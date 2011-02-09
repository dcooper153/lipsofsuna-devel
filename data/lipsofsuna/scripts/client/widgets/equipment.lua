require "client/widgets/frame"

Widgets.Equipment = Class(Widgets.Frame)

Widgets.Equipment.new = function(clss, args)
	local self = Widgets.Frame.new(clss, {cols = 2, pressed = args.pressed, style = "equipment"})
	self:set_expand{col = 2}
	-- TODO: Different formats for different species using args.spec.
	self.dict_name = {}
	self.dict_name["head"] = Widgets.ItemButton{pressed = function() self:pressed("head") end}
	self.dict_name["upperbody"] = Widgets.ItemButton{pressed = function() self:pressed("upperbody") end}
	self.dict_name["hand.L"] = Widgets.ItemButton{pressed = function() self:pressed("hand.L") end}
	self.dict_name["hand.R"] = Widgets.ItemButton{pressed = function() self:pressed("hand.R") end}
	self.dict_name["lowerbody"] = Widgets.ItemButton{pressed = function() self:pressed("lowerbody") end}
	self.dict_name["feet"] = Widgets.ItemButton{pressed = function() self:pressed("feet") end}
	self.dict_name["arms"] = Widgets.ItemButton{pressed = function() self:pressed("arms") end}
	self:append_row(self.dict_name["head"])
	self:append_row(self.dict_name["upperbody"])
	self:append_row(self.dict_name["hand.R"])
	self:append_row(self.dict_name["hand.L"])
	self:append_row(self.dict_name["lowerbody"])
	self:append_row(self.dict_name["feet"])
	self:append_row(self.dict_name["arms"])
	return self
end

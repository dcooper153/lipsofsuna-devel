Widgets.Equipment = Class(Widget)

Widgets.Equipment.new = function(clss, args)
	local self = Widget.new(clss, {cols = 2, pressed = args.pressed})
	self:set_expand{col = 2}
	self:set_request{internal = true, width = 256, height = 165}
	self.spacings = {0,2}
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

Widgets.Equipment.reshaped = function(self)
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "equipment1",
		source_position = {0,0},
		source_tiling = {0,256,0,0,256,0}}
	self:canvas_compile()
end

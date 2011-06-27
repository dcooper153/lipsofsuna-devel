require "client/widgets/menu"

Views.Admin = Class(Widgets.Menu)

Views.Admin.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 2, spacings = {0, 0}})
	self.title = Widgets.Title{text = "Admin stats",
		back = function() self:back() end,
		close = function() Client:set_mode("game") end,
		help = function() Client.views.help:show("admin") end}
	self.label = Widgets.Text()
	self:set_child{col = 1, row = 1, widget = self.title}
	self:set_child{col = 1, row = 2, widget = self.label}
	return self
end

Views.Admin.enter = function(self, from, level)
	Gui.menus:open{level = level, widget = self}
	Network:send{packet = Packet(packets.ADMIN_STATS)}
end

Views.Admin.back = function(self)
	Client:set_mode("menu")
end

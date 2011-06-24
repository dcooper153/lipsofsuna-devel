Client = Class()
Client.class_name = "Client"

-- Initialize the database.
-- FIXME: Should be initialized in the initializer.
Client.db = Database{name = "client.sqlite"}
Client.db:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT PRIMARY KEY,value TEXT);")

Client.init = function(self)
	-- Initialize the world.
	self.sectors = Sectors{database = Client.db, save_objects = false}
	self.sectors:erase_world()
	-- Initialize views.
	self.views = {}
	self.views.admin = Views.Admin()
	self.views.book = Views.Book()
	self.views.chargen = Views.Chargen()
	self.views.controls = Views.Controls()
	self.views.feats = Views.Feats()
	self.views.game = Views.Game()
	self.views.help = Views.Help()
	self.views.inventory = Views.Inventory()
	self.views.menu = Views.Menu()
	self.views.options = Views.Options()
	self.views.quests = Views.Quests()
	self.views.skills = Views.Skills{main = true, sync = true}
	self.views.startup = Views.Startup()
end

Client.set_mode = function(self, mode, level)
	-- Check for state changes.
	if self.mode == mode then return end
	-- Close the old view.
	Target:cancel()
	Drag:cancel()
	if self.view and self.view.close then self.view:close() end
	if Widgets.ComboBox.active then Widgets.ComboBox.active:close() end
	-- Maintain dialog visibility.
	-- Dialogs don't fit most of the time so only show them in game and menu modes.
	if mode == "game" or mode == "menu" then
		Gui.group_dialog.visible = true
	else
		Gui.group_dialog.visible = false
	end
	-- Open the new view.
	local from = self.mode
	self.mode = mode
	if mode == "chargen" or mode == "startup" then
		self.view = self.views[mode]
	else
		Gui.menus:open{level = level or 1, widget = self.views[mode]}
		self.view = self.views[mode]
	end
	if self.view and self.view.enter then self.view:enter(from) end
end

Client:add_class_getters{
	player_object = function(self) return Player.object end}

Client:add_class_setters{
	player_object = function(self, v)
		Client.views.skills:set_species(v.spec)
		self.views.feats:set_race(v.spec.name)
		self.views.feats:show(1)
		Player.object = v
		Player.species = v.spec.name
		Camera.mode = "third-person"
	end}

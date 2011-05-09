require "client/widgets/menu"

Views.Help = Class(Widgets.Menu)
Views.Help.mode = "help"

Views.Help.new = function(clss)
	local fmt = function(t) return string.gsub(t, "([^\n])\n([^\n])", "%1 %2") end
	-- About.
	local label_about = Widgets.Text{text = fmt(
[[This version of Lips of suna is pre-alpha, and as such unstable and
incomplete. Although there are several known bugs and severe lack
of content, we hope you're able to see what the game is aiming for

Visit our website at http://lipsofsuna.org to get more information
and to contact the developers. The wiki is definitely worth
checking out since it has lots of information on the project.
Although the forums are a bit quiet, you'll certainly get an answer
if you post there.]])}
	-- Hints.
	local label_hints = Widgets.Text{text = fmt(
[[You can activate NPCs, obstacles, and items by looking them and pressing
the spacebar. You can collect crafting materials, access the crafting system,
loot corpses, and discover quests by activating various things.

Crafting is done with a workbench. Access the workbench, place materials
inside it, and select the item you want to craft from the list. As your
first task, you might want to craft spare workbenches that you can take
with you.]])}
	-- Contributing.
	local label_contributing = Widgets.Text{text = fmt(
[[If the game, in its current modest state, managed to spark your interest,
you are encouraged to give us feedback or to contribute. You can contact
us by going to the project home page at http://lipsofsuna.org and clicking
the forums link.

We are looking for contributors. We especially need gentlemen who enjoy
arts such as 3D modeling, sound engineering, and quest writing. Coders
interested in writing gameplay scripts in Lua are also needed badly. We
can certainly find something to do for other people too so the lack of
work isn't an obstacle at least.]])}
	-- Main.
	local self = Widgets.Menu.new(clss, {text = "Help",
		{"About", label_about},
		{"Hints", label_hints},
		{"Contributing", label_contributing}})
	return self
end

Views.Help.back = function(self)
	Gui:set_mode("menu")
end

Views.Help.inst = Views.Help()

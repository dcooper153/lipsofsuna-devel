Help = Class()

Help.init = function(clss)
	-- About.
	clss.group_about = Widget{cols = 1}
	clss.label_about = Widgets.Label{text =
"This version of Lips of suna is pre-alpha, and as such unstable and " ..
"incomplete. Although there are several known bugs and severe lack " ..
"of content, we hope you're able to see what the game is aiming for.\n\n" ..

"Visit our website at http://lipsofsuna.org to get more information " ..
"and to contact the developers. The wiki is definitely worth " ..
"checking out since it has lots of information on the project. " ..
"Although the forums are a bit quiet, you'll certainly get an answer " ..
"if you post there."}
	clss.label_about:set_request{width = 400}
	clss.group_about:append_row(clss.label_about)
	-- Controls.
	clss.group_controls = Widget{cols = 1}
	clss.group_controls:append_row(Widgets.Label{text =
[[* Escape: Open or close the menu
* W: Walk forward
* A: Sidestep left
* S: Walk backward
* D: Sidestep right
* C: Jump
* Y: Toggle camera mode
* Space: Activate (use/read/loot/chat)
* Comma: Pick up
* Shift: Run
* Left mouse button: Attack
* Mouse wheel: Zoom camera
* Left control: Camera rotation mode]]})
	-- Hints.
	clss.group_hints = Widget{cols = 1}
	clss.group_hints:append_row(Widgets.Label{text =
[[You can activate NPCs, obstacles, and items by pressing space and
clicking on the object of interest. You can collect crafting materials,
access the crafting system, and discover quests by activating various
things.]]})
	-- Contributing.
	clss.group_contributing = Widget{cols = 1}
	clss.group_contributing:append_row(Widgets.Label{text =
[[If the game, in its current modest state, managed to spark your interest,
you are encouraged to give us feedback or to contribute. You can contact
us by going to the project home page at http://lipsofsuna.org and clicking
the forums link.

We are looking for contributors. We especially need gentlemen who enjoy
arts such as 3D modeling, sound engineering, and quest writing. Coders
interested in writing gameplay scripts in Lua are also needed badly. We
can certainly find something to do for other people too so the lack of
work isn't an obstacle at least.]]})
	-- Main.
	clss.menu = Widgets.Menu{
		{"About", clss.group_about},
		{"Controls", clss.group_controls},
		{"Hints", clss.group_hints},
		{"Contributing", clss.group_contributing}}
end

Help:init()

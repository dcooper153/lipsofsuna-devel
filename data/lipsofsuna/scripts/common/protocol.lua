Protocol = Class()
Protocol.types = {}

--- Registers a new packet handler.
-- @param clss Protocol class.
-- @param args Arguments.<ul>
--   <li>type: Message type.</li>
--   <li>func: Handler function.</li></ul>
Protocol.add_handler = function(clss, args)
	-- Find message number.
	local msg = packets[args.type]
	if not msg then return end
	-- Find handler group.
	local t = clss.types[msg]
	if not t then
		t = {}
		clss.types[msg] = t
	end
	-- Allocate handler ID.
	local id = 1
	while t[id] do id = math.random(1, 65535) end
	-- Add to handler group.
	t[id] = args.func
end

Eventhandler{type = "packet", func = function(self, args)
	local t = Protocol.types[args.message]
	if t then
		for k,v in pairs(t) do
			v(args)
		end
	end
end}

packets = {}
do
	local index = 0
	local function add(name)
		packets[name] = index
		index = index + 1
	end
	add("ADMIN_DELETE")
	add("ADMIN_DUPLICATE")
	add("ADMIN_SAVE")
	add("ADMIN_SHUTDOWN")
	add("ADMIN_SPAWN")
	add("APPLY")
	add("BOOK")
	add("CHARACTER_ACCEPT")
	add("CHARACTER_CREATE")
	add("CHAT")
	add("CRAFTING")
	add("CROUCH")
	add("DIALOG_ANSWER")
	add("DIALOG_CHOICE")
	add("DIALOG_CLOSE")
	add("DIALOG_LINE")
	add("EFFECT")
	add("EDIT_OBJECT_CREATE")
	add("EDIT_REGION_ENABLE")
	add("EDIT_REGION_DELETE")
	add("EDIT_REGION_DISABLE")
	add("EDIT_REGION_SAVE")
	add("EDIT_TILE")
	add("EXAMINE")
	add("FEAT")
	add("FEAT_UNLOCK")
	add("GENERATOR_STATUS")
	add("INVENTORY_CREATED")
	add("INVENTORY_CLOSED")
	add("INVENTORY_ITEM_ADDED")
	add("INVENTORY_ITEM_REMOVED")
	add("JUMP")
	add("MESSAGE")
	add("MODIFIER_ADD")
	add("MODIFIER_REMOVE")
	add("MOVE_ITEM")
	add("OBJECT_ANIMATED")
	add("OBJECT_EFFECT")
	add("OBJECT_HIDDEN")
	add("OBJECT_MOVED")
	add("OBJECT_SELF")
	add("OBJECT_SHOWN")
	add("OBJECT_SKILL")
	add("OBJECT_SLOT")
	add("OBJECT_SPEECH")
	add("PARTICLE_RAY")
	add("PLAYER_MOVE")
	add("PLAYER_TURN")
	add("QUEST_MARKER")
	add("QUEST_STATUS")
	add("RUN")
	add("SHOOT")
	add("SKILLS")
	add("STRAFE")
	add("TARGET")
	add("USE")
	add("VOXEL_DIFF")
	add("WORLD_EFFECT")
end

effects = {}
do
	local index = 0
	local function add(name)
		effects[name] = index
		index = index + 1
	end
	add("DEFAULT")
	add("EXPLOSION")
	add("EVAPORATE")
	add("HEAL")
	add("SQUISH")
end

moveitem = { INVENTORY = 0, EQUIPMENT = 1, WORLD = 2 }

Animation = {}
Animation.CHANNEL_WALK = 1
Animation.CHANNEL_EQUIP_LEFT = 2
Animation.CHANNEL_EQUIP_RIGHT = 3
Animation.CHANNEL_CROUCH = 4
Animation.CHANNEL_STRAFE = 5
Animation.CHANNEL_ACTION = 6
Animation.CHANNEL_JUMP = 7
Animation.CHANNEL_LEFT_HAND = 125
Animation.CHANNEL_RIGHT_HAND = 126
Animation.CHANNEL_TILT = 127
Animation.CHANNEL_CUSTOMIZE = 128

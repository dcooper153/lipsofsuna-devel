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
	add("ADMIN_SAVE")
	add("ADMIN_SHUTDOWN")
	add("ADMIN_SPAWN")
	add("ADMIN_PRIVILEGE")
	add("ADMIN_STATS")
	add("EDIT_TILE") -- FIXME: ADMIN_TILE

	add("AUTHENTICATE_REJECT")

	add("BOOK")
	add("CHARACTER_ACCEPT")
	add("CHARACTER_CREATE")
	add("CLIENT_AUTHENTICATE")

	add("CRAFTING")

	add("DIALOG_ANSWER")
	add("DIALOG_CHOICE")
	add("DIALOG_CLOSE")
	add("DIALOG_LINE")

	add("EFFECT")
	add("WORLD_EFFECT") -- FIXME: EFFECT_WORLD

	add("FEAT")
	add("FEAT_UNLOCK")

	add("GENERATOR_STATUS")

	add("INVENTORY_CREATED")
	add("INVENTORY_CLOSED")
	add("INVENTORY_ITEM_ADDED")
	add("INVENTORY_ITEM_REMOVED")

	add("MESSAGE")

	add("MODIFIER_ADD")
	add("MODIFIER_REMOVE")

	add("MOVE_ITEM") -- FIXME: Ugly

	add("OBJECT_ANIMATED")
	add("OBJECT_BEHEADED")
	add("OBJECT_DEAD")
	add("OBJECT_EFFECT")
	add("OBJECT_FEAT")
	add("OBJECT_HIDDEN")
	add("OBJECT_MOVED")
	add("OBJECT_SHOWN")
	add("OBJECT_SKILL")
	add("OBJECT_SLOT")
	add("OBJECT_SPEECH")

	add("QUEST_MARKER")
	add("QUEST_STATUS")

	add("PLAYER_BLOCK")
	add("CHAT") -- FIXME: PLAYER_CHAT
	add("PLAYER_CLIMB")
	add("EXAMINE") -- FIXME: PLAYER_EXAMINE
	add("JUMP") -- FIXME: PLAYER_JUMP
	add("PLAYER_MOVE")
	add("PLAYER_RESPAWN")
	add("RUN") -- FIXME: PLAYER_RUN
	add("SHOOT") -- FIXME: PLAYER_SHOOT
	add("SKILLS") -- FIXME: PLAYER_SKILLS
	add("STRAFE") -- FIXME: PLAYER_STRAFE
	add("PLAYER_TURN")
	add("USE") -- FIXME: PLAYER_TARGET
	add("PLAYER_WEIGHT")

	add("TRADING_ACCEPT")
	add("TRADING_ADD_BUY")
	add("TRADING_ADD_SELL")
	add("TRADING_CANCEL")
	add("TRADING_REMOVE_BUY")
	add("TRADING_REMOVE_SELL")
	add("TRADING_START")

	add("VOXEL_DIFF")
end

Protocol.object_flags = {
	SPEEDLINE = 0x1,
	BEHEADED = 0x2}

Protocol.object_show_flags = {
	ANIMS = 0x000100,
	APPEARANCE = 0x000200,
	NAME = 0x000400,
	POSITION = 0x000800,
	ROTATION = 0x001000,
	SELF = 0x002000,
	SKILLS = 0x004000,
	SLOTS = 0x008000,
	SPEC = 0x010000,
	SPECIES = 0x020000}

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
Animation.CHANNEL_TILT = 127
Animation.CHANNEL_CUSTOMIZE = 128

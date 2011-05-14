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
	add("CLIENT_AUTHENTICATE")
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
	add("OBJECT_DEAD")
	add("OBJECT_EFFECT")
	add("OBJECT_FEAT")
	add("OBJECT_HIDDEN")
	add("OBJECT_MOVED")
	add("OBJECT_SELF")
	add("OBJECT_SHOWN")
	add("OBJECT_SKILL")
	add("OBJECT_SLOT")
	add("OBJECT_SPEECH")
	add("PARTICLE_RAY")
	add("PLAYER_BLOCK")
	add("PLAYER_MOVE")
	add("PLAYER_TURN")
	add("PLAYER_WEIGHT")
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
	add("ADMIN_PRIVILEGE")
	add("AUTHENTICATE_REJECT")
	add("ADMIN_STATS")
	add("OBJECT_BEHEADED")
end

Protocol.object_flags = {
	SPEEDLINE = 0x1,
	BEHEADED = 0x2}

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

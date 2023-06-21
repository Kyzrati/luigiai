
//==================================================================
// Luigi's AI reference data
//==================================================================
// struct implementations mostly found in BS.cpp

bool luigiAiActive = false; // allows data to be loaded, set true with "-luigiAi" command line switch
bool luigiAiTest = false; // if true, then every action the entire map area within FOV is output to FILE_LUIGIAI_TEST (set w/"-luigiAiTest" command line switch)

#define DIRECTORY_LUIGIAI_ONLY	(string() + "luigiAi/")
#define DIRECTORY_LUIGIAI		(CUSTOM_FILE_PATH + "luigiAi/")
#define FILE_LUIGIAI_CELLID		(CUSTOM_FILE_PATH + "luigiAi/cellID.txt")
#define FILE_LUIGIAI_PROPID		(CUSTOM_FILE_PATH + "luigiAi/propID.txt")
#define FILE_LUIGIAI_ENTITYID	(CUSTOM_FILE_PATH + "luigiAi/entityID.txt")
#define FILE_LUIGIAI_ITEMID		(CUSTOM_FILE_PATH + "luigiAi/itemID.txt")
#define FILE_LUIGIAI_TEST		(CUSTOM_FILE_PATH + "luigiAi/test.txt")

struct LuigiMachineHacking
{
	int actionReady;	// this number is incremented each time Cogmind is ready
						//	and waiting for input for a new hacking command
						// (0 = not yet ready for first input)
	int detectChance;	// chance to be detected (only applicable before trace begins)
	int	traceProgress;	// current trace amount from 0-100
	bool lastHackSuccess;// set true if last hack succeeded

	LuigiMachineHacking(int detectChance_, int traceProgress_)
		: actionReady		(0)
		, detectChance		(detectChance_)
		, traceProgress		(traceProgress_)
		, lastHackSuccess	(false)
	{};
};

struct LuigiProp
{
	propID	ID;
	bool	interactivePiece;	// true if this is an interactive piece,
								//	and not currently disabled

	LuigiProp() {};
	void load(HProp prop);
};

struct LuigiItem
{
	itemID	ID;
	int		integrity;
	bool	equipped;	// whether currently equipped, where if true
						//	assumed to be owned by a LuigiEntity

	LuigiItem() {};
	void load(HItem item);
};

struct LuigiEntity
{
	entityID				ID;
	int						integrity;
	factionRelationType		relation;
	entityActiveStateType	activeState;
							// variables loaded for all Entities technically
							//	but most intended for Cogmind/player use
	int						exposure;
	int						energy;
	int						matter;
	int						heat;
	int						systemCorruption;
	int						speed;
	int						inventorySize; // current length of inventory array
	LuigiItem*				inventory;

	LuigiEntity()
		: inventory	(NULL)
	{};
	void load(HEntity entity);
};

struct LuigiTile
{
	int				lastAction;	// set to LuigiAi::actionReady value
								//	of the last time anything at this
								//	location *may* have been updated
								//	(i.e. if this doesn't match that value
								//	can safely ignore it if you want,
								//	though old data is still retained)
	int				lastFov;	// set to LuigiAi::actionReady value
								//	of that last time this cell was in FOV
								//	(hence can be used to know whether
								//	currently in FOV or not)
	cellID			cell;		// NO_CELL (-1) if unknown
	bool			doorOpen;	// true if a confirmed open door,
								//	otherwise closed (or not a door)
					// objects (NULL if N/A)
	LuigiProp*		prop;
	LuigiEntity*	entity;
	LuigiItem*		item;

	LuigiTile()
		: lastAction	(0)
		, lastFov		(0)
		, cell			(NO_CELL)
		, doorOpen		(false)
		, prop			(NULL)
		, entity		(NULL)
		, item			(NULL)
	{};
	~LuigiTile()
	{
		delete prop;
		delete entity;
		delete item;
	};
};

struct LuigiAi
{
	int magic1;			// magic number to locate beginning of memory block
	int magic2;			// second magic number for confirmation
	int actionReady;	// this number is incremented each time Cogmind is ready
						//	and waiting for input for a new action
						// (0 = run has not yet begun)
					// updated at start of each new map
	int				mapWidth,		// full dimensions of current map
					mapHeight;
	int				locationDepth;	// current depth on world map
	mapType			locationMap;	// current map (see .proto MapType for values)
					// updated immediately before each action
	LuigiTile*		mapData;		// data for all map cells
									//	(length = mapWidth*mapHeight)
									//	(access = x*mapHeight+y)
	int				mapCursorIndex;	// mapData index of current cursor position
									// (-1 if not currently positioned over map)
	LuigiEntity*	player;			// player/Cogmind data
					// special interfaces
	LuigiMachineHacking* machineHacking;// actively used only while CHack UI open,
									// otherwise NULL
	void initialize()
	{
		magic1			= 1689123404;
		magic2			= 2035498713;
		actionReady		= 0;
		mapWidth		= 0;
		mapHeight		= 0;
		locationDepth	= -11;
		locationMap		= MAP_JUNKYARD;
		mapData			= NULL;
		mapCursorIndex	= -1;
		player			= NULL;
		machineHacking	= NULL;
	};
	void loadMap();
	void cleanup()
	{
		delete [] mapData;		mapData = NULL;
		delete player;			player = NULL;
		delete machineHacking;	machineHacking = NULL;
	};
	// LuigiAI data is not serialized, and loading a game will
	//	only be able to restore information about what is already in FOV
};
LuigiAi luigiAi;

void updateLuigiAiMapTile(Cell* cell, bool includeItem);
#include "overlay.h"
#include "SDK.h"
#include <thread>

std::vector<player_t> entities = {};
std::vector<item_t> items = {};

void init()
{
	globals.process_id = FindProcessID("DayZ_x64.exe");
	globals.handle = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, globals.process_id);

	globals.Base = GetModule("DayZ_x64.exe");
	globals.World = ReadData<uint64_t>(globals.Base + OFF_WORLD);
	
	std::thread(update_list).detach();
}

void update_list()
{
	while (true)
	{
		// stores all data
		std::vector<player_t> tmp{};

		uint64_t NearTableSize = ReadData<uint32_t>(globals.World + OFF_NEAR_ENTITY_TABLE + 0x8);
		uint64_t FarTableSize = ReadData<uint32_t>(globals.World + OFF_FAR_ENTITY_TABLE + 0x8);

		for (int i = 0; i < NearTableSize; i++)
		{
			uint64_t EntityTable = ReadData<uint64_t>(globals.World + OFF_NEAR_ENTITY_TABLE);
			if (!EntityTable) continue;

			uint64_t Entity = ReadData<uint64_t>(EntityTable + (i * 0x8));
			if (!Entity) continue;

			// checking if player even networked
			uintptr_t networkId = ReadData<uintptr_t>(Entity + OFF_NETWORK);
			//if (networkId == 0) continue;

			player_t Player{};
			Player.NetworkID = networkId;
			Player.TableEntry = EntityTable;
			Player.EntityPtr = Entity;

			// adds info to the vector
			tmp.push_back(Player);
		}

		for (int i = 0; i < FarTableSize; i++)
		{
			uint64_t EntityTable = ReadData<uint64_t>(globals.World + OFF_FAR_ENTITY_TABLE);
			if (!EntityTable) continue;

			uint64_t Entity = ReadData<uint64_t>(EntityTable + (i * 0x8));
			if (!Entity) continue;

			uintptr_t networkId = ReadData<uintptr_t>(Entity + OFF_NETWORK);
			//if (networkId == 0) continue;

			player_t Player{};
			Player.NetworkID = networkId;
			Player.TableEntry = EntityTable;
			Player.EntityPtr = Entity;
			tmp.push_back(Player);
		}
		entities = tmp;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void draw_esp()
{
	for (int i = 0; i < entities.size(); i++)
	{
		auto Entities = entities[i];

		Vector3 worldPosition = Game::GetCoordinate(Entities.EntityPtr);
		Vector3 screenPosition;
		
		Game::WorldToScreen(worldPosition, screenPosition);

		if (screenPosition.z < 1.0f) continue;

		int distance = Game::GetDistanceToMe(worldPosition);

		if (screenPosition.z >= 1.0f)
		{
			std::string entity = Game::GetEntityTypeName(Entities.EntityPtr);

			// Player
			if (entity == "dayzplayer" && distance <= 1000.f)
			{
				DrawPlayerBar(screenPosition.x, screenPosition.y + 25, &Col.glassyellow, &Col.black, ("[" + std::to_string(distance) + "m] - P").c_str());
			}

			// Zombie
			else if (entity == "dayzinfected" && distance <= 150.f)
			{
				DrawPlayerBar(screenPosition.x, screenPosition.y + 25, &Col.glassblack, &Col.white, ("[" + std::to_string(distance) + "m] - Z").c_str());

			}

			// Car
			else if(entity == "car" && distance <= 800.f)
			{
				DrawStringWithBackGround(globals.Width / 2, globals.Height / 2 - 500, &Col.glassblack, &Col.red, "There are cars nearby!");
				DrawPlayerBar(screenPosition.x, screenPosition.y + 25, &Col.glassblack, &Col.darkgreen, ("[" + std::to_string(distance) + "m] - Car").c_str());
			}
		}
	}
}
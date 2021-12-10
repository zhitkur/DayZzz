#pragma once
#include "vector.h"
#include "memory.h"
#include <string>

namespace Game
{
	uint64_t NearEntityTable()
	{
		return ReadData<uint64_t>(globals.World + 0xEB8);
	}

	uint32_t NearEntityTableSize()
	{
		return ReadData<uint32_t>(globals.World + 0xEB8 + 0x08);
	}

	uint64_t GetEntity(uint64_t PlayerList, uint64_t SelectedPlayer)
	{
		// Sorted Object
		return ReadData<uint64_t>(PlayerList + SelectedPlayer * 0x8);
	}

	std::string GetEntityTypeName(uint64_t Entity)
	{
		// Render Entity Type + Config Name
		return ReadArmaString(ReadData<uint64_t>(ReadData<uint64_t>(Entity + 0x148) + 0xA0)).c_str();
	}

	uint64_t FarEntityTable()
	{
		return ReadData<uint64_t>(globals.World + 0x1000);
	}

	uint32_t FarEntityTableSize()
	{
		return ReadData<uint32_t>(globals.World + 0x1000 + 0x08);
	}

	uint64_t GetCameraOn()
	{
		// Camera On 
		return ReadData<uint64_t>(globals.World + 0x28A8);
	}

	uint64_t GetLocalPlayer()
	{
		// Sorted Entity Object
		return ReadData<uint64_t>(ReadData<uint64_t>(globals.World + 0x28B8) + 0x8) - 0xA8;
	}

	uint64_t GetLocalPlayerVisualState()
	{
		// Future Visual State
		return ReadData<uint64_t>(GetLocalPlayer() + 0x198);
	}

	Vector3 GetCoordinate(uint64_t Entity)
	{
		while (true)
		{
			if (Entity == GetLocalPlayer())
			{
				return Vector3(ReadData<Vector3>(GetLocalPlayerVisualState() + 0x2C));
			}
			else
			{
				return  Vector3(ReadData<Vector3>(ReadData<uint64_t>(Entity + 0x198) + 0x2C));
			}
		}
	}
	uint64_t GetCamera()
	{
		while (true)
		{
			return ReadData<uint64_t>(globals.World + 0x1B8);
		}
	}

	Vector3 GetInvertedViewTranslation()
	{
		return Vector3(ReadData<Vector3>(GetCamera() + 0x2C));
	}
	Vector3 GetInvertedViewRight()
	{
		return Vector3(ReadData<Vector3>(GetCamera() + 0x8));
	}
	Vector3 GetInvertedViewUp()
	{
		return Vector3(ReadData<Vector3>(GetCamera() + 0x14));
	}
	Vector3 GetInvertedViewForward()
	{
		return Vector3(ReadData<Vector3>(GetCamera() + 0x20));
	}

	Vector3 GetViewportSize()
	{
		return Vector3(ReadData<Vector3>(GetCamera() + 0x58));
	}

	Vector3 GetProjectionD1()
	{
		return Vector3(ReadData<Vector3>(GetCamera() + 0xD0));
	}

	Vector3 GetProjectionD2()
	{
		return Vector3(ReadData<Vector3>(GetCamera() + 0xDC));
	}


	uint32_t GetSlowEntityTableSize()
	{
		return ReadData<uint32_t>(globals.World + OFF_SLOW_ENTITY_TABLE + 0x08);
	}

	float GetDistanceToMe(Vector3 Entity)
	{
		return Entity.Distance(GetCoordinate(GetLocalPlayer()));
	}

	bool WorldToScreen(Vector3 Position, Vector3& output)
	{
		if (!GetCamera()) return false;

		Vector3 temp = Position - GetInvertedViewTranslation();

		float x = temp.Dot(GetInvertedViewRight());
		float y = temp.Dot(GetInvertedViewUp());
		float z = temp.Dot(GetInvertedViewForward());

		if (z < 0.1f)
			return false;

		Vector3 res(
			GetViewportSize().x * (1 + (x / GetProjectionD1().x / z)),
			GetViewportSize().y * (1 - (y / GetProjectionD2().y / z)),
			z);

		output.x = res.x;
		output.y = res.y;
		output.z = res.z;
		return true;
	}
}
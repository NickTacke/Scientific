#pragma once
#include "../../../Utils/Memory.h"

class CVector;
class CUserCmd;
class SpatialQuery;

class IEngineClient
{
public:
	constexpr std::int32_t GetLocalPlayerIndex()
	{
		return Memory::vmt::Call<std::int32_t>(this, 12);
	}

	constexpr void SetViewAngles(const CVector& viewAngles)
	{
		Memory::vmt::Call<void, const CVector&>(this, 19, viewAngles);
	}

	SpatialQuery* GetBSPTreeQuery()
	{
		return Memory::vmt::Call<SpatialQuery*>(this, 43);
	}
};
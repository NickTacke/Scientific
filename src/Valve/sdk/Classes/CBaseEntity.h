#pragma once
#include "../../../Utils/Memory.h"

class CVector;

class CBaseEntity
{
public:
	constexpr std::int32_t GetTeam()
	{
		return Memory::vmt::Call<std::int32_t>(this, 88);
	}

	constexpr bool IsAlive()
	{
		return Memory::vmt::Call<bool>(this, 156);
	}

	constexpr bool IsPlayer()
	{
		return Memory::vmt::Call<bool>(this, 158);
	}

	constexpr void GetEyePosition(CVector& eyePositionOut)
	{
		Memory::vmt::Call<void, CVector&>(this, 285, eyePositionOut);
	}

	constexpr void GetAimPunch(CVector& aimPunchOut)
	{
		Memory::vmt::Call<void, CVector&>(this, 346, aimPunchOut);
	}

	int GetFlags()
	{
		return *reinterpret_cast<int*>(uintptr_t(this) + 0x104);
	}
};
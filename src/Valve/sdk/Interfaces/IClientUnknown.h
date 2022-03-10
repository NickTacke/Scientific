#pragma once
#include "../../../Utils/Memory.h"

class CBaseEntity;

class IClientUnknown
{
public:
	CBaseEntity* GetBaseEntity()
	{
		return Memory::vmt::Call<CBaseEntity*>(this, 7);
	}
};
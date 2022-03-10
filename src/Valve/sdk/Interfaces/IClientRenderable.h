#pragma once
#include "../../../Utils/Memory.h"

class IClientUnknown;

class IClientRenderable
{
public:
	IClientUnknown* GetIClientUnknown()
	{
		return Memory::vmt::Call<IClientUnknown*>(this, 0);
	}
};
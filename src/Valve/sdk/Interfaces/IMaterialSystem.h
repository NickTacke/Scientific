#pragma once
#include "../../../Utils/Memory.h"

class IMaterial;

class IMaterialSystem
{
public:
	constexpr IMaterial* FindMaterial(const char* name) noexcept
	{
		return Memory::vmt::Call<IMaterial*>(this, 84, name, nullptr, true, nullptr);
	}
};
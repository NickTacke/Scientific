#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>

namespace Memory
{
	void Setup();

	namespace vmt
	{
		template <typename Return, typename ... Arguments>
		constexpr Return Call(void* vmt, const std::uint32_t index, Arguments ... args)
		{
			using Function = Return(__thiscall*)(void*, decltype(args)...);
			return (*static_cast<Function**>(vmt))[index](vmt, args...);
		}
	}

	namespace Pattern
	{
		std::uint8_t* Scan(const char* moduleName, const char* pattern) noexcept;
	}

	inline std::uint8_t* allocKeyValuesClient = nullptr;
	inline std::uint8_t* allocKeyValuesEngine = nullptr;
}
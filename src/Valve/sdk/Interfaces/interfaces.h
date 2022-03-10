#pragma once
#include <Windows.h>

#include "IClientEntityList.h"
#include "IStudioRender.h"
#include "IMaterialSystem.h"
#include "IEngineClient.h"
#include "IEngineTraceClient.h"

namespace Interfaces
{
	inline IClientEntityList* iClientEntityList = nullptr;
	inline IStudioRender* iStudioRender = nullptr;
	inline IMaterialSystem* iMaterialSystem = nullptr;
	inline IEngineClient* iEngineClient = nullptr;
	inline IEngineTraceClient* iEngineTraceClient = nullptr;

	inline void* client = nullptr;
	inline void* clientMode = nullptr;

	inline void* keyValuesSystem = nullptr;

	void Setup();

	template <typename Interface>
	Interface* GetInterface(const char* moduleName, const char* interfaceName)
	{
		HMODULE moduleHandle = GetModuleHandle(moduleName);

		if (!moduleHandle)
			return nullptr;

		FARPROC functionAddress = GetProcAddress(moduleHandle, "CreateInterface");

		if (!functionAddress)
			return nullptr;

		using CreateInterfaceFn = Interface * (*)(const char* interfaceName, int* returnCode);
		CreateInterfaceFn CreateInterface = reinterpret_cast<CreateInterfaceFn>(functionAddress);

		return CreateInterface(interfaceName, NULL);
	}
}
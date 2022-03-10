#include "interfaces.h"

void Interfaces::Setup()
{
	iClientEntityList = GetInterface<IClientEntityList>("client.dll", "VClientEntityList003");
	iStudioRender = GetInterface<IStudioRender>("studiorender.dll", "VStudioRender026");
	iMaterialSystem = GetInterface<IMaterialSystem>("materialsystem.dll", "VMaterialSystem080");
	iEngineClient = GetInterface<IEngineClient>("engine.dll", "VEngineClient014");
	iEngineTraceClient = GetInterface<IEngineTraceClient>("engine.dll", "EngineTraceClient004");

	client = GetInterface<void>("client.dll", "VClient018");
	clientMode = **reinterpret_cast<void***>((*reinterpret_cast<unsigned int**>(client))[10] + 5);
	
	if (const HINSTANCE handle = GetModuleHandle("vstdlib.dll"))
		keyValuesSystem = reinterpret_cast<void* (__cdecl*)()>(GetProcAddress(handle, "KeyValuesSystem"))();
}
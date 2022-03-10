#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <thread>

#include "Valve/sdk/Interfaces/interfaces.h"
#include "Valve/sdk/Hooks/hooks.h"

#include "globals.h"

void Setup(HMODULE hModule)
{
	// Check if game is running
	Globals::ClientBase = (DWORD)GetModuleHandle("client.dll");
	if (!Globals::ClientBase)
		return;

	Globals::EngineBase = (DWORD)GetModuleHandle("engine.dll");
	if (!Globals::EngineBase)
		return;

	// Setups
	Memory::Setup();
	Overlay::Setup();
	Interfaces::Setup();
	Hooks::Setup();

	// Cheat loop
	while (!GetAsyncKeyState(VK_END))
	{
		if (GetAsyncKeyState(VK_INSERT) & 1)
			Overlay::open = !Overlay::open;

		Globals::localPlayer = *(DWORD*)(Globals::ClientBase + 0xdb35dc);

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	// Destroyers
	Overlay::Destroy();
	Hooks::Destroy();

	// Uninject
	FreeLibraryAndExitThread(hModule, NULL);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		const HANDLE SetupThread = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Setup), hModule, NULL, nullptr);

		if (SetupThread)
			CloseHandle(SetupThread);
	}

	return TRUE;
}
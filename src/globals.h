#include <Windows.h>

namespace Globals
{
	inline DWORD ClientBase = NULL;
	inline DWORD EngineBase = NULL;

	inline DWORD localPlayer = NULL;

	inline bool chams = false;
	inline float teamHidden[3] = { 0.000f, 0.294f, 1.000f };
	inline float teamVisible[3] = { 0.407f, 0.756f, 1.000f };

	inline float enemiesHidden[3] = { 0.816f, 0.000f, 0.000f };
	inline float enemiesVisible[3] = { 0.980f, 0.488f, 0.404f };

	inline bool triggerBot = false;

	inline bool bhop = false;
}
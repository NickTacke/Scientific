#pragma once
#include "../sdk.h"

namespace Hooks
{
	void Setup();
	void Destroy();

	constexpr void* VirtualFunction(void* _this, size_t index)
	{
		return (*static_cast<void***>(_this))[index];
	}

	// EndScene

	using EndSceneFn = long(__thiscall*)(void*, IDirect3DDevice9*);
	inline EndSceneFn oEndScene = nullptr;

	long __stdcall EndScene(IDirect3DDevice9* device);

	// CreateMove

	using CreateMoveFn = bool(__thiscall*)(void*, float, CUserCmd*);
	inline CreateMoveFn oCreateMove = nullptr;

	bool __stdcall CreateMove(float frameTime, CUserCmd* cmd);

	// AllocKeyValuesMemory
	using AllocKeyValuesMemoryFn = void* (__thiscall*)(void*, const std::int32_t) noexcept;
	inline AllocKeyValuesMemoryFn AllocKeyValuesMemoryOriginal;

	void* __stdcall AllocKeyValuesMemory(const std::int32_t size) noexcept;

	// DrawModel

	using DrawModelFn = void(__thiscall*)(
		void*,
		void*,
		const CDrawModelInfo&,
		CMatrix3x4*,
		float*,
		float*,
		const CVector&,
		const std::int32_t
	) noexcept;
	inline DrawModelFn oDrawModel = nullptr;

	void __stdcall DrawModel(
		void* results,
		const CDrawModelInfo& info,
		CMatrix3x4* bones,
		float* flexWeights,
		float* flexDelayedWeights,
		const CVector& modelOrigin,
		const std::int32_t flags
	) noexcept;

	// ListLeavesInBox
	using ListLeavesInBoxFn = int(__thiscall*)(
		void*,
		CVector&,
		CVector&,
		unsigned short*,
		int
		) noexcept;
	inline ListLeavesInBoxFn oListLeavesInBox = nullptr;

	int __fastcall ListLeavesInBox(void* bsp, void* edx, CVector& mins, CVector& maxs, unsigned short* pList, int listMax);
}
#include "hooks.h"

#include <intrin.h>

#include "../../../../ext/minhook/minhook.h"

#include "../../../../ext/imgui/imgui.h"
#include "../../../../ext/imgui/imgui_impl_win32.h"
#include "../../../../ext/imgui/imgui_impl_dx9.h"

#include "../../../globals.h"


void Hooks::Setup()
{
	MH_Initialize();

	MH_CreateHook(
		VirtualFunction(Interfaces::keyValuesSystem, 1),
		&AllocKeyValuesMemory,
		reinterpret_cast<void**>(&AllocKeyValuesMemoryOriginal)
	);

	MH_CreateHook(
		VirtualFunction(Overlay::device, 42),
		&EndScene,
		reinterpret_cast<void**>(&oEndScene)
	);

	MH_CreateHook(
		VirtualFunction(Interfaces::clientMode, 24),
		&CreateMove,
		reinterpret_cast<void**>(&oCreateMove)
	);

	MH_CreateHook(
		VirtualFunction(Interfaces::iStudioRender, 29),
		&DrawModel,
		reinterpret_cast<void**>(&oDrawModel)
	);

	MH_CreateHook(
		VirtualFunction(Interfaces::iEngineClient->GetBSPTreeQuery(), 6),
		&ListLeavesInBox,
		reinterpret_cast<void**>(&oListLeavesInBox)
	);

	MH_EnableHook(MH_ALL_HOOKS);
}

void Hooks::Destroy()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}

long __stdcall Hooks::EndScene(IDirect3DDevice9* device)
{
	static const void* returnAddress = _ReturnAddress();

	const long result = oEndScene(device, device);

	if (_ReturnAddress() == returnAddress)
		return result;

	if (!Overlay::setup)
		Overlay::SetupMenu(device);

	if (Overlay::open)
		Overlay::Render();
}

void* __stdcall Hooks::AllocKeyValuesMemory(const std::int32_t size) noexcept
{
	// if function is returning to speficied addresses, return nullptr to "bypass"
	if (const std::uint32_t address = reinterpret_cast<std::uint32_t>(_ReturnAddress());
		address == reinterpret_cast<std::uint32_t>(Memory::allocKeyValuesEngine) ||
		address == reinterpret_cast<std::uint32_t>(Memory::allocKeyValuesClient))
		return nullptr;

	// return original
	return AllocKeyValuesMemoryOriginal(Interfaces::keyValuesSystem, size);
}

bool __stdcall Hooks::CreateMove(float frameTime, CUserCmd* cmd)
{
	const bool result = oCreateMove(Interfaces::clientMode, frameTime, cmd);

	if (!cmd || !cmd->commandNumber)
		return result;

	if (result)
		Interfaces::iEngineClient->SetViewAngles(cmd->viewAngles);

	CBaseEntity* LocalPlayer = Interfaces::iClientEntityList->GetClientEntity(Interfaces::iEngineClient->GetLocalPlayerIndex());

	if (!LocalPlayer || !LocalPlayer->IsAlive())
		return false;

	if (Globals::bhop)
	{
		if(!(LocalPlayer->GetFlags() & 1))
			cmd->buttons &= ~IN_JUMP;
	}

	if (!GetAsyncKeyState(VK_XBUTTON2) || !Globals::triggerBot)
		return false;

	CVector eyePosition;
	LocalPlayer->GetEyePosition(eyePosition);

	CVector aimPunch;
	LocalPlayer->GetAimPunch(aimPunch);

	const CVector distance = eyePosition + CVector{ cmd->viewAngles + aimPunch }.ToVector() * 8196.f;

	CTrace trace;
	Interfaces::iEngineTraceClient->TraceRay({ eyePosition, distance }, 0x46004009, LocalPlayer, trace);

	if (!trace.entity || !trace.entity->IsPlayer())
		return false;

	if (!trace.entity->IsAlive() || trace.entity->GetTeam() == LocalPlayer->GetTeam())
		return false;

	cmd->buttons |= IN_ATTACK;

	return false;
}

void __stdcall Hooks::DrawModel(
	void* results, 
	const CDrawModelInfo& info, 
	CMatrix3x4* bones, 
	float* flexWeights, 
	float* flexDelayedWeights, 
	const CVector& modelOrigin, 
	const std::int32_t flags
) noexcept
{
	if (Globals::localPlayer && info.renderable && Globals::chams)
	{
		CBaseEntity* localEntity = Interfaces::iClientEntityList->GetClientEntity(Interfaces::iEngineClient->GetLocalPlayerIndex());
		CBaseEntity* entity = info.renderable->GetIClientUnknown()->GetBaseEntity();

		if (entity && entity->IsPlayer())
		{
			static IMaterial* material = Interfaces::iMaterialSystem->FindMaterial("debug/debugambientcube");

			Interfaces::iStudioRender->SetAlphaModulation(1.f);

			if (entity->GetTeam() == localEntity->GetTeam())
			{
				material->SetMaterialVarFlag(IMaterial::IGNOREZ, true);
				Interfaces::iStudioRender->SetColorModulation(Globals::teamHidden);
				Interfaces::iStudioRender->ForcedMaterialOverride(material);
				oDrawModel(Interfaces::iStudioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);

				material->SetMaterialVarFlag(IMaterial::IGNOREZ, false);
				Interfaces::iStudioRender->SetColorModulation(Globals::teamVisible);
				Interfaces::iStudioRender->ForcedMaterialOverride(material);
				oDrawModel(Interfaces::iStudioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
			}
			else
			{
				material->SetMaterialVarFlag(IMaterial::IGNOREZ, true);
				Interfaces::iStudioRender->SetColorModulation(Globals::enemiesHidden);
				Interfaces::iStudioRender->ForcedMaterialOverride(material);
				oDrawModel(Interfaces::iStudioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);

				material->SetMaterialVarFlag(IMaterial::IGNOREZ, false);
				Interfaces::iStudioRender->SetColorModulation(Globals::enemiesVisible);
				Interfaces::iStudioRender->ForcedMaterialOverride(material);
				oDrawModel(Interfaces::iStudioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
			}

			return Interfaces::iStudioRender->ForcedMaterialOverride(nullptr);
		}
	}

	oDrawModel(Interfaces::iStudioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
}

#define MAX_COORD_FLOAT ( 16384.0f )
#define MIN_COORD_FLOAT ( -16384.0f )

int __fastcall Hooks::ListLeavesInBox(void* bsp, void* edx, CVector& mins, CVector& maxs, unsigned short* pList, int listMax)
{
	if (!Globals::chams && *(uint32_t*)_ReturnAddress() != 0x14244489)
		return oListLeavesInBox(edx, mins, maxs, pList, listMax);

	RenderableInfo_t* info = *(RenderableInfo_t**)((uintptr_t)_AddressOfReturnAddress() + 0x14);

	if (!info || !info->m_pRenderable)
		return oListLeavesInBox(edx, mins, maxs, pList, listMax);

	CBaseEntity* entity = info->m_pRenderable->GetIClientUnknown()->GetBaseEntity();

	if (!entity || !entity->IsPlayer())
		return oListLeavesInBox(edx, mins, maxs, pList, listMax);

	info->m_Flags &= ~0x100;
	info->m_Flags2 |= 0xC0;

	static CVector map_min = CVector(MIN_COORD_FLOAT, MIN_COORD_FLOAT, MIN_COORD_FLOAT);
	static CVector map_max = CVector(MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT);

	auto count = oListLeavesInBox(edx, map_min, map_max, pList, listMax);
	return count;
}
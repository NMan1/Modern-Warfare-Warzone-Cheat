#include "Global.h"
#include "stdafx.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"

DWORD64 OFFSET_RefDef = 0x106A4C60;
DWORD64 OFFSET_LocalPlayer = 0x106afaa0;

namespace Globals
{
	DWORD64 pGameBase = NULL;
	HANDLE hWindow = NULL;
	HMODULE hMainModule = NULL;
	Camera* pRefDef;

	Camera* GetRefDef()
	{
		return (Camera*)*(uintptr_t*)(Globals::pGameBase + OFFSET_RefDef);
	}

	uint64_t GetLocalPlayer()
	{
		return *(uint64_t*)(Globals::pGameBase + OFFSET_LocalPlayer);
	}

	Vector GetLocalPlayerPos()
	{
		return *(const Vector*)(Globals::GetLocalPlayer() + 0x30);
	}

	bool IsLocalAlive()
	{
		auto pLocal = Globals::GetLocalPlayer();
		auto vLocalPos = *(const Vector*)(pLocal + 0x30);
		return !(vLocalPos.x == 0);
	}

	void HooksInit()
	{
		if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success) 
		{
			kiero::bind(54, (void**)&d3d12hook::oExecuteCommandListsD3D12, d3d12hook::hookExecuteCommandListsD3D12);
			kiero::bind(58, (void**)&d3d12hook::oSignalD3D12, d3d12hook::hookSignalD3D12);
			kiero::bind(140, (void**)&d3d12hook::oPresentD3D12, d3d12hook::hookPresentD3D12);
			kiero::bind(84, (void**)&d3d12hook::oDrawInstancedD3D12, d3d12hook::hookkDrawInstancedD3D12);
			kiero::bind(85, (void**)&d3d12hook::oDrawIndexedInstancedD3D12, d3d12hook::hookDrawIndexedInstancedD3D12);
		}
	}
}
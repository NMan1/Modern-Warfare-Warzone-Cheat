#pragma once
#include <Windows.h>
#include <stdint.h>
#include "Camera.h"

namespace Globals
{
	extern DWORD64 pGameBase;
	extern HANDLE hWindow;
	extern HMODULE hMainModule;

	extern Camera* pRefDef;
	Camera* GetRefDef();
	uint64_t GetLocalPlayer();
	Vector GetLocalPlayerPos();
	bool IsLocalAlive();

	void HooksInit();
}

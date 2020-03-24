#include "stdafx.h"
#include "Entity.h"
#include "Settings.h"

unsigned long __stdcall onAttach()
{
	AllocConsole();
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	SetConsoleTitleA("Overflow");
	MoveWindow(GetConsoleWindow(), 50, 50, 410, 410, TRUE);
	ShowWindow(::GetConsoleWindow(), SW_HIDE);

	Globals::pGameBase = reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));
	Utils::Log("Game base found:   ", Globals::pGameBase);

	while (!(Globals::hWindow = FindWindowA(NULL, "Call of Duty®: Modern Warfare®")));
	Utils::Log("Game window found: ", Globals::hWindow);

	GetPid();
	Globals::HooksInit();

	do 
	{
		Sleep(100);

		if (GetAsyncKeyState(VK_NUMPAD1))
			Settings::bEspToggle = !Settings::bEspToggle;

		if (GetAsyncKeyState(VK_NUMPAD2))
			Settings::bTextToggle = !Settings::bTextToggle;

		if (GetAsyncKeyState(VK_NUMPAD3))
			Settings::bSnapLinesToggle = !Settings::bSnapLinesToggle;
		
		if (GetAsyncKeyState(VK_NUMPAD0))
			ShowWindow(::GetConsoleWindow(), SW_SHOW);

	} while (!GetAsyncKeyState(VK_END));

	
	d3d12hook::release();
	kiero::shutdown();
	Menu::InputRemove((HWND)Globals::hWindow);

	FreeConsole();
	FreeLibraryAndExitThread(Globals::hMainModule, 0);
	
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		Globals::hMainModule = hModule;
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)onAttach, hModule, 0, 0);
	}
	
	return 1;
}
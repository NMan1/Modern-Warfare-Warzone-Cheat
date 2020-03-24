#pragma once

namespace Menu
{
	extern bool bIsOpen;
	void DrawMenu();
	void MenuInit();
	void InputInit(HWND hWindow);
	void InputRemove(HWND hWindow);
	LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}
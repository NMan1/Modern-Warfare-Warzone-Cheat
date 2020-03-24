#pragma once
#include <Windows.h>
#include "imgui.h"
#include "Render.h"
#include "Global.h"
#include <Settings.h>

namespace Menu
{
	bool bIsOpen = true;

	ImVec4 Red = { 255, 0, 0, 255 };
	ImVec4 Green = { 0, 255, 0, 255 };

	void MenuInit()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.f;
		style.WindowPadding = ImVec2(28, 14);
		style.WindowMinSize = ImVec2(32, 32);
		style.WindowRounding = 0.0f; //4.0 for slight curve
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.ChildRounding = 0.0f;
		style.FramePadding = ImVec2(4, 3);
		style.FrameRounding = 0.0f; //2.0
		style.ItemSpacing = ImVec2(8, 4);
		style.ItemInnerSpacing = ImVec2(4, 4);
		style.TouchExtraPadding = ImVec2(0, 0);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 3.0f;
		style.ScrollbarSize = 6.0f;
		style.ScrollbarRounding = 16.0f; //16.0
		style.GrabMinSize = 0.1f;
		style.GrabRounding = 16.0f; //16.00
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.DisplayWindowPadding = ImVec2(22, 22);
		style.DisplaySafeAreaPadding = ImVec2(4, 4);
		style.AntiAliasedLines = true;
		style.CurveTessellationTol = 1.25f;
	}


	void DrawMenu()
	{
		Menu::MenuInit();

		ImGuiWindowFlags	window_flags = 0;
		ImGui::SetNextWindowSize(ImVec2(130, 140));
		ImGui::SetNextWindowPos(ImVec2(1775, 15));

		ImGui::PushFont(m_pTahomaFont);
		ImGui::Begin("Overflow", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{
			if (Settings::bEspToggle)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, Green);
				//ImGui::Checkbox("ESP", &Globals::bEspToggle);
				ImGui::Text("ESP  - ON");
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, Red);
				ImGui::Text("ESP  - OFF");
			}

			ImGui::PopStyleColor();
			ImGui::Spacing();

			if (Settings::bTextToggle)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, Green);
				ImGui::Text("Text  - ON");
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, Red);
				ImGui::Text("Text  - OFF");
			}

			ImGui::PopStyleColor();
			ImGui::Spacing();
			
			if (Settings::bSnapLinesToggle)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, Green);
				ImGui::Text("Snap - ON");
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, Red);
				ImGui::Text("Snap - OFF");
			}

			ImGui::PopStyleColor();
			ImGui::Spacing();

		}
		ImGui::End();
		ImGui::PopFont();
	}

	WNDPROC	oWndProc;

	LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
			ImGui::GetIO().MouseDown[0] = true; return DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		case WM_LBUTTONUP:
			ImGui::GetIO().MouseDown[0] = false; return DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		case WM_RBUTTONDOWN:
			ImGui::GetIO().MouseDown[1] = true; return DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		case WM_RBUTTONUP:
			ImGui::GetIO().MouseDown[1] = false; return DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		case WM_MBUTTONDOWN:
			ImGui::GetIO().MouseDown[2] = true; return DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		case WM_MBUTTONUP:
			ImGui::GetIO().MouseDown[2] = false; return DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		case WM_MOUSEWHEEL:
			ImGui::GetIO().MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f; return DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		case WM_MOUSEMOVE:
			ImGui::GetIO().MousePos.x = (signed short)(lParam); ImGui::GetIO().MousePos.y = (signed short)(lParam >> 16); return DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		}

		return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
	}

	void InputInit(HWND hWindow)
	{
		oWndProc = (WNDPROC)SetWindowLongPtr(hWindow, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
	}

	void InputRemove(HWND hWindow)
	{
		SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)oWndProc);
	}
}
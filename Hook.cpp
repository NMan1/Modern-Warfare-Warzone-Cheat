#include "stdafx.h"
#include <iostream>
#include <imgui\imgui_internal.h>
#include "Render.h"
#include "Features.h"
#include "tahoma.ttf.h"
#include <Camera.h>
#include <Entity.h>
#include <Settings.h>

namespace d3d12hook {
	ID3D12Device* d3d12Device = nullptr;
	ID3D12DescriptorHeap* d3d12DescriptorHeapBackBuffers = nullptr;
	ID3D12DescriptorHeap* d3d12DescriptorHeapImGuiRender = nullptr;
	ID3D12GraphicsCommandList* d3d12CommandList = nullptr;
	ID3D12Fence* d3d12Fence = nullptr;
	UINT64 d3d12FenceValue = 0;
	ID3D12CommandQueue* d3d12CommandQueue = nullptr;

	PresentD3D12 oPresentD3D12;
	DrawInstancedD3D12 oDrawInstancedD3D12;
	DrawIndexedInstancedD3D12 oDrawIndexedInstancedD3D12;

	void(*oExecuteCommandListsD3D12)(ID3D12CommandQueue*, UINT, ID3D12CommandList*);
	HRESULT(*oSignalD3D12)(ID3D12CommandQueue*, ID3D12Fence*, UINT64);

	struct __declspec(uuid("189819f1-1db6-4b57-be54-1821339b85f7")) ID3D12Device;

	struct FrameContext {
		ID3D12CommandAllocator* commandAllocator = nullptr;
		ID3D12Resource* main_render_target_resource = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE main_render_target_descriptor;
	};

	uint32_t buffersCounts = -1;
	FrameContext* frameContext;

	bool shutdown = false;

	long __fastcall hookPresentD3D12(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags) 
	{
		static bool init = false;

		if (GetAsyncKeyState(VK_INSERT) & 0x1) 
			Menu::bIsOpen = !Menu::bIsOpen;

		if (!init) 
		{
			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&d3d12Device))) 
			{
				ImGui::CreateContext();
				
				unsigned char* pixels;
				int width, height;
				ImGuiIO& io = ImGui::GetIO(); (void)io;
				ImGui::StyleColorsDark();
				io.Fonts->AddFontDefault();
				io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
				io.IniFilename = NULL;

				CreateEvent(nullptr, false, false, nullptr);

				DXGI_SWAP_CHAIN_DESC sdesc;
				pSwapChain->GetDesc(&sdesc);
				sdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
				sdesc.OutputWindow = (HWND)Globals::hWindow;
				sdesc.Windowed = ((GetWindowLongPtr((HWND)Globals::hWindow, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

				buffersCounts = sdesc.BufferCount;
				frameContext = new FrameContext[buffersCounts];

				D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
				descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				descriptorImGuiRender.NumDescriptors = buffersCounts;
				descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

				if (d3d12Device->CreateDescriptorHeap(&descriptorImGuiRender, IID_PPV_ARGS(&d3d12DescriptorHeapImGuiRender)) != S_OK)
					return false;
				
				ID3D12CommandAllocator* allocator;
				if (d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)) != S_OK)
					return false;

				for (size_t i = 0; i < buffersCounts; i++) {
					frameContext[i].commandAllocator = allocator;
				}

				if (d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL, IID_PPV_ARGS(&d3d12CommandList)) != S_OK ||
					d3d12CommandList->Close() != S_OK)
					return false;

				D3D12_DESCRIPTOR_HEAP_DESC descriptorBackBuffers;
				descriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				descriptorBackBuffers.NumDescriptors = buffersCounts;
				descriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				descriptorBackBuffers.NodeMask = 1;

				if (d3d12Device->CreateDescriptorHeap(&descriptorBackBuffers, IID_PPV_ARGS(&d3d12DescriptorHeapBackBuffers)) != S_OK)
					return false;

				const auto rtvDescriptorSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12DescriptorHeapBackBuffers->GetCPUDescriptorHandleForHeapStart();

				for (size_t i = 0; i < buffersCounts; i++) {
					ID3D12Resource* pBackBuffer = nullptr;

					frameContext[i].main_render_target_descriptor = rtvHandle;
					pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
					d3d12Device->CreateRenderTargetView(pBackBuffer, nullptr, rtvHandle);
					frameContext[i].main_render_target_resource = pBackBuffer;
					rtvHandle.ptr += rtvDescriptorSize;
				}

				ImGui_ImplWin32_Init((HWND)Globals::hWindow);
				ImGui_ImplDX12_Init(d3d12Device, buffersCounts,
					DXGI_FORMAT_R8G8B8A8_UNORM, d3d12DescriptorHeapImGuiRender,
					d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart(),
					d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart());

				ImFont* m_pFont = io.Fonts->AddFontFromMemoryTTF(tahoma_ttf, sizeof(tahoma_ttf), 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
				ImFont* m_pFontLarge = io.Fonts->AddFontFromMemoryTTF(tahoma_ttf, sizeof(tahoma_ttf), 24.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
				g_Renderer->Initialize(m_pFont, m_pFontLarge);

				ImGui_ImplDX12_CreateDeviceObjects();

				//Menu::InputInit((HWND)Globals::hWindow);
			}
			init = true;
		}

		if (!shutdown) 
		{
			if (d3d12CommandQueue == nullptr)
				return oPresentD3D12(pSwapChain, SyncInterval, Flags);

			static float rainbow = 0.f;
			if (rainbow > 1.f)
				rainbow = 0.f;

			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			if (Menu::bIsOpen)
				Menu::DrawMenu();

			g_Renderer->BeginScene();
			auto Color = FromHSB(rainbow, 1.f, 1.f);
			g_Renderer->RenderText("OVERFLOW", ImVec2(10, 6), 28, ImColor(Color.x, Color.y , Color.z , 255.f), false, true);

			if (Settings::bEspToggle && Globals::IsLocalAlive())
				ESP::Draw();

			g_Renderer->EndScene();
			
			FrameContext& currentFrameContext = frameContext[pSwapChain->GetCurrentBackBufferIndex()];
			currentFrameContext.commandAllocator->Reset();

			D3D12_RESOURCE_BARRIER barrier;
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = currentFrameContext.main_render_target_resource;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

			d3d12CommandList->Reset(currentFrameContext.commandAllocator, nullptr);
			d3d12CommandList->ResourceBarrier(1, &barrier);
			d3d12CommandList->OMSetRenderTargets(1, &currentFrameContext.main_render_target_descriptor, FALSE, nullptr);
			d3d12CommandList->SetDescriptorHeaps(1, &d3d12DescriptorHeapImGuiRender);

			ImGui::Render();
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12CommandList);

			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

			d3d12CommandList->ResourceBarrier(1, &barrier);
			d3d12CommandList->Close();

			d3d12CommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&d3d12CommandList));
			rainbow += 0.0007f;
		}
		return oPresentD3D12(pSwapChain, SyncInterval, Flags);
	}

	void __fastcall hookkDrawInstancedD3D12(ID3D12GraphicsCommandList* dCommandList, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) 
	{
		return oDrawInstancedD3D12(dCommandList, VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
	}

	void __fastcall hookDrawIndexedInstancedD3D12(ID3D12GraphicsCommandList* dCommandList, UINT IndexCount, UINT InstanceCount, UINT StartIndex, INT BaseVertex) 
	{
		return oDrawIndexedInstancedD3D12(dCommandList, IndexCount, InstanceCount, StartIndex, BaseVertex);
	}

	void hookExecuteCommandListsD3D12(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists) 
	{
		if (!d3d12CommandQueue)
			d3d12CommandQueue = queue;

		oExecuteCommandListsD3D12(queue, NumCommandLists, ppCommandLists);
	}

	HRESULT hookSignalD3D12(ID3D12CommandQueue* queue, ID3D12Fence* fence, UINT64 value)
	{
		if (d3d12CommandQueue != nullptr && queue == d3d12CommandQueue) 
		{
			d3d12Fence = fence;
			d3d12FenceValue = value;
		}

		return oSignalD3D12(queue, fence, value);
	}

	void release() 
	{
		shutdown = true;
		kiero::unbind(54);
		kiero::unbind(58);
		kiero::unbind(140);
		kiero::unbind(84);
		kiero::unbind(85);
	}
}
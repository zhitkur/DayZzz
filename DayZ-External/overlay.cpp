#include <iostream>
#include <thread>
#include "overlay.h"
#include "font.h"

IDirect3D9Ex* p_Object = NULL;
IDirect3DDevice9Ex* p_Device = NULL;
D3DPRESENT_PARAMETERS p_Params = { NULL };

void ov::create_window(bool console)
{
	if (!console)
		ShowWindow(GetConsoleWindow(), SW_HIDE);

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),
		0,
		WndProc,
		0,
		0,
		nullptr,
		LoadIcon(0, IDI_APPLICATION),
		LoadCursor(0, IDC_ARROW),
		nullptr,
		nullptr,
		globals.lWindowName,
		LoadIcon(nullptr, IDI_APPLICATION)
	};

	if (!RegisterClassEx(&wcex))
		return;

	globals.TargetWnd = FindWindowA(NULL, "DayZ");

	if (globals.TargetWnd)
	{
		RECT tSize;

		GetWindowRect(globals.TargetWnd, &tSize);
		globals.Width = tSize.right - tSize.left;
		globals.Height = tSize.bottom - tSize.top;

		globals.OverlayWnd = CreateWindowEx(
			NULL,
			globals.lWindowName,
			globals.lWindowName,
			WS_POPUP | WS_VISIBLE,
			tSize.left,
			tSize.top,
			globals.Width,
			globals.Height,
			NULL,
			NULL,
			0,
			NULL
		);

		const MARGINS Margin = { 0, 0, globals.Width, globals.Height };

		DwmExtendFrameIntoClientArea(globals.OverlayWnd, &Margin);
		SetWindowLong(globals.OverlayWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
		ShowWindow(globals.OverlayWnd, SW_SHOW);

	}

#ifdef _DEBUG
	std::cout << "Success create window.." << std::endl;
#endif

	setup_directx(globals.OverlayWnd);
}

void ov::setup_directx(HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = globals.Width;
	p_Params.BackBufferHeight = globals.Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
	{
		p_Object->Release();
		exit(4);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(p_Device);

	io.Fonts->AddFontFromMemoryCompressedTTF(Cousine_compressed_data, Cousine_compressed_size, 15.f);
	p_Object->Release();
}

void ov::update_overlay()
{
	HWND hwnd_active = GetForegroundWindow();
	if (hwnd_active == globals.TargetWnd)
	{
		HWND temp_hwnd = GetWindow(hwnd_active, GW_HWNDPREV);
		SetWindowPos(globals.OverlayWnd, temp_hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

void ov::render()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	update_overlay();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0)); // Invisible 
	ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
	ImGui::Begin(("##scene"), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);

	int tWidth = globals.Width;	int tHeight = globals.Height;

	DrawString(16, globals.Width / 2, globals.Height / 2 - 400, &Col.lightblue, true, true, "Overlay by zhitkur");

	draw_esp();

	ImGui::GetWindowDrawList()->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);

	ImGui::EndFrame();
	p_Device->SetRenderState(D3DRS_ZENABLE, false);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (p_Device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		p_Device->EndScene();
	}
	HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		p_Device->Reset(&p_Params);
		ImGui_ImplDX9_CreateDeviceObjects();
	}

}

WPARAM ov::loop()
{
	MSG msg;

	while (true)
	{
		ZeroMemory(&msg, sizeof(MSG));
		if (::PeekMessage(&msg, globals.OverlayWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT || GetAsyncKeyState(VK_DELETE) || FindWindowA(NULL, "DayZ") == NULL)
			exit(0);

		ov::render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	clean_directx();
	DestroyWindow(globals.OverlayWnd);

	return msg.wParam;
}

void ov::clean_directx()
{
	if (p_Device != NULL)
	{
		p_Device->EndScene();
		p_Device->Release();
	}
	if (p_Object != NULL)
	{
		p_Object->Release();
	}
}
#include "gui.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/style.h"
#include <dwmapi.h>
#include "../../src/header.h"
#include <string>
#include "../../globals/params.h"

ImFont* font1;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

LRESULT CALLBACK WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED)
		{
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			gui::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter); // set click points
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(
					gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}

	}return 0;

	}

	return DefWindowProc(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName) noexcept
{

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = WS_EX_TRANSPARENT;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(NULL);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.lpszMenuName = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszClassName = "class001";
	windowClass.hIconSm = 0;

	MARGINS margins = { -1 };

	// Create and populate the blur-behind structure.
	DWM_BLURBEHIND bb = { 0 };

	// Specify blur-behind and blur region.
	bb.dwFlags = DWM_BB_ENABLE;
	bb.fEnable = true;
	bb.hRgnBlur = NULL;

	RegisterClassEx(&windowClass);

	window = CreateWindowExA(
		0,
		"class001",
		windowName,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);
	DwmExtendFrameIntoClientArea(window, &margins); //make transparent
	//DwmEnableBlurBehindWindow(window, &bb);
	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}

void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;
	InitStyle();
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
	//font1 = io.Fonts->AddFontFromFileTTF("../font/Arya.tff", 18.0f);

}

void gui::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT)
		{
			isRunning = !isRunning;
			return;
		}
	}

	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 0),0, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}

void gui::Render() noexcept
{

	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	InitStyle();
	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGui::Begin(
		"SEX",
		&isRunning,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove	|
		ImGuiWindowFlags_NoTitleBar 
		//ImGuiWindowFlags_NoBackground
	);
	auto drawList = ImGui::GetWindowDrawList();
	drawList->AddTriangleFilled(ImVec2(-50, 20), ImVec2(80, 19), ImVec2(80, 21), IM_COL32(237, 220, 145, 240));

	drawList->AddTriangleFilled(ImVec2(100, 21), ImVec2(100, 19), ImVec2(230, 20), IM_COL32(237, 220, 145, 240)); // rgba(237, 220, 145, 1)

	drawList->AddTriangleFilled(ImVec2(-50, 75), ImVec2(80, 74), ImVec2(80, 76), IM_COL32(237, 220, 145, 240));

	drawList->AddTriangleFilled(ImVec2(100, 76), ImVec2(100, 74), ImVec2(230, 75), IM_COL32(237, 220, 145, 240)); // rgba(237, 220, 145, 1)

	//draw text
	
	/*draw apm*/
	//ImGui::PushFont(font1);
	std::string text = "APM";
	if (gl_params::apm != 0)
		text = text + ": " + std::to_string(gl_params::apm);

	ImVec2 input = ImGui::CalcTextSize(text.c_str());
	drawList->AddText(ImVec2(90 - input.x/2, 45), IM_COL32(196, 161, 91, 255), text.c_str());						//rgba(196, 161, 91, 1)

	/*draw cps*/
	
	std::string text2 = "afksleep";
	//text2 = text2 + ": " + std::to_string(gl_params::cps);

	ImVec2 input2 = ImGui::CalcTextSize(text2.c_str());
	drawList->AddText(ImVec2(90 - input2.x / 2, 30), IM_COL32(196, 161, 91, 255), text2.c_str());
	

	ImGui::NewLine();
	ImGui::End();
}
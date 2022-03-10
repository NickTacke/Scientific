#include "Overlay.h"

#include "../../../../ext/imgui/imgui.h"
#include "../../../../ext/imgui/imgui_impl_win32.h"
#include "../../../../ext/imgui/imgui_impl_dx9.h"

#include <stdexcept>

#include "../../../globals.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParam, LPARAM longParam);

// window process
LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wideParam, LPARAM longParam);


void Overlay::Render() noexcept
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Scientific", &open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::Checkbox("Chams", &Globals::chams);

	if (Globals::chams)
	{
		ImGui::Text("Team");
		ImGui::ColorEdit3("Visible##ChamsTeam", Globals::teamVisible, ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs);
		ImGui::ColorEdit3("Hidden##ChamsTeam", Globals::teamHidden, ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs);

		ImGui::Text("Enemies");
		ImGui::ColorEdit3("Visible##ChamsEnemy", Globals::enemiesVisible, ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs);
		ImGui::ColorEdit3("Hidden##ChamsEnemy", Globals::enemiesHidden, ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs);
	}

	ImGui::Checkbox("Triggerbot", &Globals::triggerBot);

	ImGui::Checkbox("Bhop", &Globals::bhop);

	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}


bool Overlay::SetupWindowClass(const char* windowClassName) noexcept
{
	// popuulate window class
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = NULL;


	// register
	if (!RegisterClassEx(&windowClass))
		return false;

	return true;
}

void Overlay::DestroyWindowClass() noexcept
{
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool Overlay::SetupWindow(const char* windowName) noexcept
{
	// create temp window
	window = CreateWindow(windowClass.lpszClassName, windowName, WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, 0, 0, windowClass.hInstance, 0);
	if (!window)
		return false;

	return true;
}

void Overlay::DestroyWindow() noexcept
{
	if (window)
		DestroyWindow(window);
}

bool Overlay::SetupDirectX() noexcept
{
	const auto handle = GetModuleHandle("d3d9.dll");
	if (!handle)
		return false;
	using CreateFn = LPDIRECT3D9(__stdcall*)(UINT);

	const auto create = reinterpret_cast<CreateFn>(GetProcAddress(handle, "Direct3DCreate9"));

	if (!create)
		return false;

	d3d9 = create(D3D_SDK_VERSION);

	if (!d3d9)
		return false;

	D3DPRESENT_PARAMETERS params = { };
	params.BackBufferWidth = 0;
	params.BackBufferHeight = 0;
	params.BackBufferFormat = D3DFMT_UNKNOWN;
	params.BackBufferCount = 0;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality = NULL;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = window;
	params.Windowed = 1;
	params.EnableAutoDepthStencil = 0;
	params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	params.Flags = NULL;
	params.FullScreen_RefreshRateInHz = 0;
	params.PresentationInterval = 0;

	if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &params, &device) < 0) return false;
	return true;
}

void Overlay::DestroyDirectX() noexcept
{
	if (device)
	{
		device->Release();
		device = NULL;
	}

	if (d3d9)
	{
		d3d9->Release();
		d3d9 = NULL;
	}
}

void Overlay::Setup()
{
	if (!SetupWindowClass("eu"))
		throw std::runtime_error("Failed To Create Window class.");

	if (!SetupWindow("eu window"))
		throw std::runtime_error("Failed To Create Window.");

	if (!SetupDirectX())
		throw std::runtime_error("Failed To Create Device.");

	DestroyWindow();
	DestroyWindowClass();
}

void Overlay::SetupMenu(LPDIRECT3DDEVICE9 device) noexcept
{
	auto params = D3DDEVICE_CREATION_PARAMETERS{ };
	device->GetCreationParameters(&params);

	window = params.hFocusWindow;

	originalWindowProcess = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowProcess)));

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

	setup = true;
}

void Overlay::Destroy() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalWindowProcess));

	DestroyDirectX();
}

LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wideParam, LPARAM longParam)
{
	if (Overlay::open && ImGui_ImplWin32_WndProcHandler(window, message, wideParam, longParam))
		return 1L;

	return CallWindowProc(Overlay::originalWindowProcess, window, message, wideParam, longParam);
}
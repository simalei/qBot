// #define QBOT_DEVMODE

#include <includes.hpp>

#include "hooks/hooks.hpp"
#include "gui/gui.hpp"

DWORD MainThread(LPVOID lpParam)
{
    #ifdef QBOT_DEVMODE
    AllocConsole();
    SetConsoleTitle("qBot debug console");
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    #endif

    MH_Initialize();

    NFD_Init();

    ImGuiHook::setInitFunction(GUI::initUI);
    ImGuiHook::setRenderFunction(GUI::renderUI);

    ImGuiHook::setToggleCallback([]() {
        GUI::visible = !GUI::visible;
    });

    ImGuiHook::setToggleKey(VK_HOME);

    ImGuiHook::setupHooks([](void* target, void* hook, void** trampoline) {
        MH_CreateHook(target, hook, trampoline);
    });

    Hooks::init(); // All GD-related hooks

    MH_EnableHook(MH_ALL_HOOKS);

	return S_OK;
}

DWORD WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0x1000, reinterpret_cast<LPTHREAD_START_ROUTINE>(&MainThread), NULL, 0, NULL);
		break;

	default:
		break;
	}

	return TRUE;
}
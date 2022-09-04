#include "hooks.hpp"
#include "hooks/PlayLayer/PlayLayer.hpp"
#include "hooks/Keybinds/dispatchKeyboardMSG.hpp"

void Hooks::init()
{
	MH_CreateHook(
		(PVOID)(gd::base + 0x01FB780),
		Hooks::PlayLayer::hkInit,
		(LPVOID*)&Hooks::PlayLayer::init
	);
	MH_CreateHook(
		(PVOID)(gd::base + 0x20A1A0),
		Hooks::PlayLayer::hkDeath,
		(LPVOID*)&Hooks::PlayLayer::death
	);
	MH_CreateHook(
		(PVOID)(gd::base + 0x20BF00),
		Hooks::PlayLayer::hkResetLevel,
		(LPVOID*)&Hooks::PlayLayer::resetLevel
	);
	MH_CreateHook(
		(PVOID)(gd::base + 0x2029C0),
		Hooks::PlayLayer::hkUpdate,
		(LPVOID*)&Hooks::PlayLayer::update
	);
	MH_CreateHook(
		(PVOID)(gd::base + 0x111500),
		Hooks::PlayLayer::hkPushButton,
		(LPVOID*)&Hooks::PlayLayer::pushButton
	);
	MH_CreateHook(
		(PVOID)(gd::base + 0x111660),
		Hooks::PlayLayer::hkReleaseButton,
		(LPVOID*)&Hooks::PlayLayer::releaseButton
	);
	MH_CreateHook(
		(PVOID)(gd::base + 0x20D810),
		Hooks::PlayLayer::hkOnQuit,
		(LPVOID*)&Hooks::PlayLayer::onQuit
    );
	MH_CreateHook(
		(PVOID)(gd::base + 0x20D0D0),
		Hooks::PlayLayer::hkTogglePractice,
		(LPVOID*)&Hooks::PlayLayer::togglePractice
    );
	MH_CreateHook(
		(PVOID)(gd::base + 0x20B050),
		Hooks::PlayLayer::hkCreateCheckpoint,
		(LPVOID*)&Hooks::PlayLayer::createCheckpoint
	);
	MH_CreateHook(
		(PVOID)(gd::base + 0x20B830),
		Hooks::PlayLayer::hkRemoveCheckpoint,
		(LPVOID*)&Hooks::PlayLayer::removeCheckpoint
	);
	MH_CreateHook(
		(PVOID)(GetProcAddress(GetModuleHandleA("libcocos2d.dll"), "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z")),
		Keybinds::hkDispatchKeyboardMSG,
		(LPVOID*)&Keybinds::dispatchKeyboardMSG
	);
}
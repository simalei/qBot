#pragma once
#include <includes.hpp>
#include "../PlayLayer/PlayLayer.hpp"

namespace Keybinds
{
    inline void(__thiscall* dispatchKeyboardMSG)(void* self, int key, bool down);
    void __fastcall hkDispatchKeyboardMSG(void* self, void*, int key, bool down);
}
#pragma once
#include <includes.hpp>
#include "../CCScheduler/CCScheduler.hpp"

namespace Hooks
{
    namespace Misc
    {
        inline void(__thiscall* dispatchKeyboardMSG)(void* self, int key, bool down);
        void __fastcall hkDispatchKeyboardMSG(void* self, void*, int key, bool down);
    }
}

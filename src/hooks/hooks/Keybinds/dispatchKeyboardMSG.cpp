#include "dispatchKeyboardMSG.hpp"

namespace Keybinds
{
    void __fastcall hkDispatchKeyboardMSG(void* self, void*, int key, bool down)
    {
        if (down)
        {
            if (key == 'R')
            {
                if (qBot::inLevel)
                {
                    Hooks::PlayLayer::resetLevel(gd::GameManager::sharedState()->getPlayLayer());
                }
            }
        }
        dispatchKeyboardMSG(self, key, down);
    }
}

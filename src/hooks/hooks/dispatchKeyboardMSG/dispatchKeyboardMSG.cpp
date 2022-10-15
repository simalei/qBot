#include "dispatchKeyboardMSG.hpp"

namespace Hooks::Misc
{
    void __fastcall hkDispatchKeyboardMSG(void* self, void*, int key, bool down)
    {
        
        if (down)
        {
            if (key == 'F')
            {
                FrameAdvance::frameAdvanceEnabled = !FrameAdvance::frameAdvanceEnabled;
            }
            if (key == 'C')
            {
                PlayLayer::update(gd::GameManager::sharedState()->getPlayLayer(), 1 / FPSMultiplier::target_fps);
                qBot::Update(gd::GameManager::sharedState()->getPlayLayer());
                FrameAdvance::advanceOneFrame = true;
            }
        }
        dispatchKeyboardMSG(self, key, down);
        
    }
} // namespace Hooks::Misc

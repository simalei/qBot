#include "FPSMultiplier.hpp"


namespace FPSMultiplier
{
    float target_fps = 60;
    bool enabled = true;
    bool disableRender = false;
    float leftOver = 0.f;
}

namespace Hooks
{
    namespace CCScheduler
    {
        void __fastcall hkUpdate(cocos2d::CCScheduler* self, int, float dt) {
            if (!FPSMultiplier::enabled)
                return update(self, dt);
            auto speedhack = CCDirector::sharedDirector()->getScheduler()->getTimeScale();
            const float newdt = 1.f / (FPSMultiplier::target_fps * speedhack);
            FPSMultiplier::disableRender = true;

            const int times = min(static_cast<int>((dt + FPSMultiplier::leftOver) / newdt), 100); // limit it to 100x just in case
            for (int i = 0; i < times; ++i) {
                if (i == times - 1)
                    FPSMultiplier::disableRender = false;
                update(self, newdt);
            }
            FPSMultiplier::leftOver += dt - newdt * times;
        }
    }

    namespace PlayLayer
    {
        void __fastcall hkUpdateVisibility(void* self) {
            if (!FPSMultiplier::disableRender)
                updateVisibility(self);
        }
    }
}
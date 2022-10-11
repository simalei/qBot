#pragma once

#include <includes.hpp>
#include "../Speedhack/Speedhack.hpp"

namespace FPSMultiplier
{
    extern float target_fps;
    extern bool enabled;
    extern bool disableRender;
    extern float leftOver;
}

namespace Hooks::CCScheduler
{
    inline void(__thiscall* update)(cocos2d::CCScheduler*, float);
    void __fastcall hkUpdate(cocos2d::CCScheduler* self, int, float dt);
}

namespace Hooks::PlayLayer
{
    inline void(__thiscall* updateVisibility)(void*);
    void __fastcall hkUpdateVisibility(void* self);
}
#include "PlayLayer.hpp"

namespace Hooks::PlayLayer
{
    bool __fastcall hkInit(gd::PlayLayer* self, int edx, void* GJGameLevel)
    {
        auto ret = init(self, GJGameLevel);
        if (!ret) {return ret;}
        Clickbot::InitClickbot();
        qBot::Init(self);
        return ret;
    }

    void __fastcall hkUpdate(gd::PlayLayer* self, void*, float delta)
    {
        update(self, delta);
        qBot::Update(self);
    }

    int __fastcall hkDeath(gd::PlayLayer* self, void*, void* go, void* powerrangers)
    {
        return death(self, go, powerrangers);
    }
    
    void __fastcall hkOnQuit(gd::PlayLayer* self)
    {
        qBot::Quit(self);
        onQuit(self);
    }

    int __fastcall hkResetLevel(gd::PlayLayer* self)
    {
        qBot::Reset(self);
        return resetLevel(self);
    }

    bool __fastcall hkPushButton(gd::PlayLayer* self, uintptr_t, int state, bool player)
    {
        
        qBot::PushButton(self, player);
        return pushButton(self, state, player);
    }

    bool __fastcall hkReleaseButton(gd::PlayLayer* self, uintptr_t, int state, bool player)
    {
        qBot::ReleaseButton(self, player);
        return releaseButton(self, state, player);
    }

    void __fastcall hkTogglePractice(void* self, int edx, bool practice)
    {
        togglePractice(self, practice);
    }

    int __fastcall hkCreateCheckpoint(void* self)
    {
        qBot::PlaceCheckpoint();
        return createCheckpoint(self);
    }

    int __fastcall hkRemoveCheckpoint(void* self)
    {
        qBot::RemoveCheckpoint();
        return removeCheckpoint(self);
    }
} // namespace Hooks::PlayLayer

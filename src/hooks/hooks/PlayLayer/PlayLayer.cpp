#include "PlayLayer.hpp"

namespace Hooks::PlayLayer
{
    bool lockDeltaEnabled = true;
    bool ignoreUserInputEnabled = false;
    bool dualClickEnabled = false;

    bool __fastcall hkInit(gd::PlayLayer* self, int edx, void* GJGameLevel)
    {
        PlayLayer::init(self, GJGameLevel);
        std::cout << "playlayer initialized" << std::endl;
        qBot::Init(self);
        
        


        return true;
    }

    void __fastcall hkUpdate(gd::PlayLayer* self, void*, float delta)
    {      
        if (lockDeltaEnabled && GUI::mode == 2)
        {
            update(self, 1 / FPSMultiplier::target_fps);
        } else {
            update(self, delta);
        }
        qBot::Update(self);
    }

    int __fastcall hkDeath(gd::PlayLayer* self, void*, void* go, void* powerrangers)
    {
        qBot::Death();
        return death(self, go, powerrangers);
    }
    
    void __fastcall hkOnQuit(gd::PlayLayer* self)
    {
        qBot::Quit(self);
        onQuit(self);
    }

    int __fastcall hkResetLevel(gd::PlayLayer* self)
    {
        qBot::frame = 0;
        resetLevel(self);
        qBot::Reset(self);
        return 0;
    }

    bool __fastcall hkPushButton(gd::PlayLayer* self, uintptr_t, int state, bool player)
    {
        qBot::PushButton(self, player);
        if (ignoreUserInputEnabled && GUI::mode == 2)
        {
            return true;
        }
        if (dualClickEnabled)
        {
            pushButton(self, state, !player);
        }
        return pushButton(self, state, player);
    }

    bool __fastcall hkReleaseButton(gd::PlayLayer* self, uintptr_t, int state, bool player)
    {
        qBot::ReleaseButton(self, player);
        if (ignoreUserInputEnabled && GUI::mode == 2)
        {
            return true;
        }
        if (dualClickEnabled)
        {
            releaseButton(self, state, !player);
        }
        
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

#pragma once

#include <includes.hpp>
#include "../../../qbot/qbot.hpp"

namespace Hooks::PlayLayer
{
    inline void(__thiscall* update)(gd::PlayLayer* self, float delta);
    void __fastcall hkUpdate(gd::PlayLayer* self, void*, float delta);
    
    inline int(__thiscall* resetLevel)(gd::PlayLayer* self);
    int __fastcall hkResetLevel(gd::PlayLayer* self);
    
    inline int(__thiscall* death)(gd::PlayLayer* self, void* go, void* powerrangers);
    int __fastcall hkDeath(gd::PlayLayer* self, void*, void* go, void* powerrangers);
    
    inline bool(__thiscall* init)(gd::PlayLayer* self, void* GJGameLevel);
    bool __fastcall hkInit(gd::PlayLayer* self, int edx, void* GJGameLevel);
    
    inline void(__thiscall* onQuit)(gd::PlayLayer* self);
	void __fastcall hkOnQuit(gd::PlayLayer* self);
    
    inline bool(__thiscall* pushButton)(gd::PlayLayer* self, int state, bool player);
    bool __fastcall hkPushButton(gd::PlayLayer* self, uintptr_t, int state, bool player);
    
    inline bool(__thiscall* releaseButton)(gd::PlayLayer* self, int state, bool player);
    bool __fastcall hkReleaseButton(gd::PlayLayer* self, uintptr_t, int state, bool player);
    
    inline void(__thiscall* togglePractice)(void* self, bool practice);
    void __fastcall hkTogglePractice(void* self, int edx, bool practice);
    
    inline int(__thiscall* createCheckpoint)(void* self);
    int __fastcall hkCreateCheckpoint(void* self);
    
    inline int(__thiscall* removeCheckpoint)(void* self);
    int __fastcall hkRemoveCheckpoint(void* self);
} // namespace PlayLayer
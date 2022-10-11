#pragma once
#include <includes.hpp>
#include <vector>
#include "../gui/gui.hpp"
#include "../hooks/hooks/PlayLayer/PlayLayer.hpp"
#include <variant>

template <typename R, typename T>
inline R cast(T value) { return reinterpret_cast<R>(value); }

namespace qBot
{
    extern std::vector<std::tuple<bool, bool, float, float, float, float, float, float, double, double, bool, bool>> vanilaMacro;
    extern std::vector<int> checkpoints;
    extern int frame;
    extern float xpos;
    extern bool inLevel;
    extern bool p1ButtonPushed;
    extern bool p2ButtonPushed;
    extern bool accuracyFixEnabled;
    extern bool showStatusEnabled;
    extern bool frameAdvanceEnabled;
    extern bool advanceOneFrame;
    extern bool fakeCheatIndicatorEnabled;
    extern std::string levelName;

    void Update(gd::PlayLayer* self);
    void Quit(gd::PlayLayer* self);
    void PushButton(gd::PlayLayer* self, bool player);
    void ReleaseButton(gd::PlayLayer* self, bool player);
    void Death();
    void Reset(gd::PlayLayer* self);
    void PlaceCheckpoint();
    void RemoveCheckpoint();
    void TogglePractice();
    void Init(gd::PlayLayer* self);
} // namespace qBot

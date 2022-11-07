#pragma once
#include <includes.hpp>
#include <vector>
#include "../gui/gui.hpp"
#include "../hooks/hooks/PlayLayer/PlayLayer.hpp"
#include "../recorder/recorder.hpp"
#include "../hooks/hooks/CCScheduler/CCScheduler.hpp"
#include <variant>

template <typename R, typename T>
inline R cast(T value) { return reinterpret_cast<R>(value); }


// frame: frame; xpos: X position; action: true if down, false if up; player: true if p1, false if p2.
struct Click {
    int fps;
    int frame;
    float xpos;
    float ypos1;
    float ypos2;
    float rotation1;
    float rotation2;
    bool action;
    bool player;
};

struct Checkpoint {
    int frame;
    float xpos;
    float ypos1;
    float ypos2;
    float rotation1;
    float rotation2;
    double yAccel1;
    double yAccel2;
};

namespace qBot
{
    extern std::vector<Click> macro;
    extern std::vector<Checkpoint> checkpoints;
    extern int frame;
    extern float xpos;
    extern bool inLevel;
    extern bool p1ButtonPushed;
    extern bool p2ButtonPushed;
    extern bool accuracyFixEnabled;
    extern bool checkpointFixEnabled;
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
    void PlaceCheckpoint(gd::PlayLayer* self);
    void RemoveCheckpoint(gd::PlayLayer* self);
    void TogglePractice();
    void Init(gd::PlayLayer* self);
} // namespace qBot

#pragma once
#define QBOT_DEVMODE
#define SAMPLE std::tuple<bool, bool, float, float>

#include <includes.hpp>
#include <fstream>
#include "../qbot/qbot.hpp"
#include "../hooks/hooks/Speedhack/Speedhack.hpp"
#include "../hooks/hooks/FPSBypass/FPSBypass.hpp"
#include "../clickbot/clickbot.hpp"

namespace GUI
{
    void initUI();
    void renderUI();

    extern bool visible;
    extern int mode;

    namespace MainWindow
    {
        void render();
    }

    namespace DebugWindow
    {
        void render();
    }

    namespace Notifications
    {
        void render();
    }
}
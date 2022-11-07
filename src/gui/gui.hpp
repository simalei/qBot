#pragma once
#include <curl/curl.h>
#include <includes.hpp>
#include <fstream>
#include <7zpp.h>
#include <misc/cpp/imgui_stdlib.h>
#include "../qbot/qbot.hpp"
#include "../hooks/hooks/Speedhack/Speedhack.hpp"
#include "../hooks/hooks/FPSBypass/FPSBypass.hpp"
#include "../hooks/hooks/CCScheduler/CCScheduler.hpp"
#include "../recorder/recorder.hpp"
#include <thread>
#include <string>

namespace GUI
{
    void initUI();
    void renderUI();

    void loadMacro(nfdchar_t* outPath);

    extern bool visible;
    extern int mode;
    extern int macroType;

    namespace MainWindow
    {
        void render();
    }

    namespace IDWindow
    {
        void render();
    }
    
    namespace Notifications
    {
        void render();
    }
}
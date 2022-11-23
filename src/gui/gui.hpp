#pragma once
#include <curl/curl.h>
#include <includes.hpp>
#include <fstream>
#include <misc/cpp/imgui_stdlib.h>
#include "../qbot/qbot.hpp"
#include "../hooks/hooks/Speedhack/Speedhack.hpp"
#include "../hooks/hooks/FPSBypass/FPSBypass.hpp"
#include "../hooks/hooks/CCScheduler/CCScheduler.hpp"
#include "../recorder/recorder.hpp"
#include "../imguiExt/imguiExt.hpp"
#include <thread>
#include <string>

namespace GUI
{
    void initUI();
    void renderUI();

    void loadMacro(std::string outPath);

    extern bool visible;
    extern int mode;
    extern std::string macroFilename;

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
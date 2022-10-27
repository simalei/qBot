#pragma once
#define SAMPLE std::tuple<bool, bool, float, float, float, float, float, float, double, double, bool, bool>
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

    extern bool visible;
    extern int mode;

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
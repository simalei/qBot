#include "gui.hpp"

namespace GUI
{
    bool visible = false;
    bool demoWindow = false;
    int mode = 0;
    nfdfilteritem_t filter[] = {{"qBot replay file", "dat"}};

    namespace MainWindow
    {
        void render()
        {
            ImGui::Begin("qBot dev-release", nullptr, ImGuiWindowFlags_NoResize);
            ImGui::SetWindowSize({279,281});

            if (ImGui::BeginTabBar("MainTabBar"))
            {
                if (ImGui::BeginTabItem("qBot"))
                {
                    ImGui::RadioButton("Disabled", &mode, 0);
                    ImGui::SameLine();
                    ImGui::RadioButton("Record", &mode, 1);
                    ImGui::SameLine();
                    ImGui::RadioButton("Playback", &mode, 2);

                    ImGui::Separator();


                    ImGui::Text("Frame:");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", qBot::frame);
                    
                    ImGui::Text("XPos:");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%f", qBot::xpos);

                    ImGui::Text("Level name:");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", qBot::levelName.c_str());
                    

                    ImGui::Separator();
                    
                    if (ImGui::Button("Save macro", {260.f, 24.f}))
                    {
                        nfdchar_t *outPath;
                        nfdresult_t result = NFD_SaveDialog(&outPath, filter, 1, "qBot\\replays", qBot::levelName.c_str());
                        if (result == NFD_OKAY)
                        {
                            std::ofstream out(outPath, std::ios::binary);
                            out.write((char*)qBot::vanilaMacro.data(),sizeof(SAMPLE)*qBot::vanilaMacro.size());
                            ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Replay successfully saved to %s", outPath});
                            FPSBypass::setFPS();
                        }
                    }
                    
                    if (ImGui::Button("Load macro", {260.f, 24.f}))
                    {
                        nfdchar_t *outPath;
                        nfdresult_t result = NFD_OpenDialog(&outPath, filter, 1, "qBot\\replays");
                        if (result == NFD_OKAY)
                        {
                            qBot::vanilaMacro.clear();
                            std::ifstream in(outPath, std::ios::binary);
                            size_t count = in.seekg(0, std::ios::end).tellg()/sizeof(SAMPLE);
                            qBot::vanilaMacro.resize(count);
                            in.seekg(0);
                            in.read((char*)qBot::vanilaMacro.data(),sizeof(SAMPLE)*count);
                            ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Replay %s successfully loaded", outPath});
                            FPSBypass::setFPS();
                        }   
                    }
                    
                    if (ImGui::Button("Clear macro", {260.f, 24.f}))
                    {
                        qBot::vanilaMacro.clear();
                    }

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Utilities"))
                {
                    ImGui::Text("Speedhack");
                    ImGui::InputFloat("Speed", &Speedhack::target_speed);
                    if (ImGui::Button("Apply speed", {260.f, 24.f}))
                    {
                        Speedhack::setSpeed();
                    }
                    
                    ImGui::Separator();

                    ImGui::Text("FPS Bypass");
                    ImGui::InputInt("FPS", &FPSBypass::target_fps, 10);
                    if (ImGui::Button("Apply FPS", {260.f, 24.f}))
                    {
                        FPSBypass::setFPS();
                    }

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Clickbot"))
                {
                    ImGui::Checkbox("Enabled", &Clickbot::enabled);
                    
                    
                    
                    // ImGui::DragInt("P1 Click volume", &Clickbot::p1ClickVolume, 1, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Settings"))
                {
                    ImGui::Text("qBot by");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Aryie", ImGui::GetVersion());

                    ImGui::Text("ImGui version:", ImGui::GetVersion());
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", ImGui::GetVersion());

                    ImGui::Separator();

                    ImGui::Text("Restart level");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "\tR");

                    ImGui::Text("Toggle frame advance");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "\tF");

                    ImGui::Text("Advance one frame");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "\tC");


                    ImGui::EndTabItem();

                }
                
                
                ImGui::EndTabBar();
            }
            
            ImGui::End();
        }
    } // namespace MainWindow

    namespace DebugWindow
    {
        void render()
        {
            ImGui::Begin("Debug");
            if (ImGui::Button("Print mode"))
            {
                std::cout << mode << std::endl;
            }
            if (ImGui::Button("Print Frame"))
            {
                std::cout << qBot::frame << std::endl;
            }
            ImGui::Checkbox("Show demo window", &demoWindow);

            if (ImGui::Button("inLevel?"))
            {
                std::cout << qBot::inLevel << std::endl;
            }
            
            
        }
    } // namespace DebugWindow
    
    namespace Notifications
    {
        void render()
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f));
            ImGui::RenderNotifications();
            ImGui::PopStyleVar(1); // Don't forget to Pop()
            ImGui::PopStyleColor(1);
        }
    } // namespace Notifications
    

    void initUI()
    {
        ImGuiIO* io = &ImGui::GetIO();
        ImFontConfig font_cfg;
        font_cfg.FontDataOwnedByAtlas = false;
        io->Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 17.f, &font_cfg);
        ImGui::MergeIconsWithLatestFont(16.f, false);
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
        colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
        colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding                     = ImVec2(8.00f, 8.00f);
        style.FramePadding                      = ImVec2(5.00f, 2.00f);
        style.CellPadding                       = ImVec2(6.00f, 6.00f);
        style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
        style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
        style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
        style.IndentSpacing                     = 25;
        style.ScrollbarSize                     = 15;
        style.GrabMinSize                       = 10;
        style.WindowBorderSize                  = 1;
        style.ChildBorderSize                   = 1;
        style.PopupBorderSize                   = 1;
        style.FrameBorderSize                   = 1;
        style.TabBorderSize                     = 1;
        style.WindowRounding                    = 7;
        style.ChildRounding                     = 4;
        style.FrameRounding                     = 3;
        style.PopupRounding                     = 4;
        style.ScrollbarRounding                 = 9;
        style.GrabRounding                      = 3;
        style.LogSliderDeadzone                 = 4;
        style.TabRounding                       = 4;
        style.WindowTitleAlign                  = ImVec2(0.5f,0.5f);

        std::cout << "GUI initialized" << std::endl;
    }

    void renderUI()
    {
        GUI::Notifications::render();
        if (visible)
        {
            GUI::MainWindow::render();
            #ifdef QBOT_DEVMODE
            GUI::DebugWindow::render();
            #endif
            if (demoWindow)
            {
                ImGui::ShowDemoWindow();
            }
            
        }
    }
} // namespace GUI




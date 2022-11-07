#include "gui.hpp"



namespace GUI {

    ImVec2 buttonSize = {282.f, 24.f};
    ImVec2 mainWindowPos;
    bool visible = false;
    bool macroEditor = false;
    ImGuiWindowFlags macroEditorFlags = 0;
    bool sequenceReplaying = false;
    bool demoWindow = false;
    int mode = 0;
    int frameOffset;
    bool isAccuracyFixMacro;
    std::string convertedMacro;
    auto& rec = Recorder::get();
    std::vector<Click> buffer;




    std::string getID()
    {
        SYSTEM_INFO siSysInfo;
        GetSystemInfo(&siSysInfo); 
        std::string id = std::string(
        std::to_string(siSysInfo.dwNumberOfProcessors) + 
        std::to_string(siSysInfo.dwPageSize) + 
        std::to_string(siSysInfo.dwProcessorType) + 
        std::to_string(siSysInfo.dwActiveProcessorMask));

        return id;
    }

    std::string id = getID();
    nfdfilteritem_t filter[] = { { "qBot frame replay", "qbf" } };
    nfdfilteritem_t plainTextFilter[] = { { "Text file", "txt" } };

    bool tobyadd = false;


    void tokenize(const std::string &s, const char delim,
                  std::vector<std::string> &out)
    {
        std::string::size_type beg = 0;
        for (auto end = 0; (end = s.find(delim, end)) != std::string::npos; ++end)
        {
            out.push_back(s.substr(beg, end - beg));
            beg = end + 1;
        }

        out.push_back(s.substr(beg));
    }

    std::string convertToPlainText(std::vector<Click> macro, int frameOffset = 0)
    {
        if (macro.size() == 0)
            return "";
        if (macro[0].xpos != -1)
        {
            ImGui::InsertNotification({ ImGuiToastType_Warning, 3000, "Macro has accuracy fix data. After applying changes, this data will be deleted!"});
        }
        std::stringstream ss;
        ss << macro[0].fps << "\n";
        for (int i = 0; i < macro.size(); ++i) {
            ss << macro[i].frame + frameOffset;
            std::string action;
            if (macro[i].player)
            {
                ss << (macro[i].action ? " 1" : " 0") << " 0\n";
            } else {
                ss << " 0 " << (macro[i].action ? "1" : "0") << "\n";
            }
        }
        return ss.str();
    }

    void convertFromPlainText(std::string plainText)
    {
        qBot::macro.clear();
        if (plainText.empty())
            return;
        int fps;
        int frame;
        bool action;
        bool player;
        std::vector<std::string> tmp;
        std::vector<std::string> tmp2;
        const char delim1 = '\n';
        const char delim2 = ' ';
        tokenize(plainText, delim1, tmp);
        fps = std::stoi(tmp[0]);
        for (size_t i = 1 /* first line is fps */; i < tmp.size() - 1; i++) {
            tmp2.clear();
            tokenize(tmp[i], delim2, tmp2);

            if (!tmp2.empty())
            {
                frame = std::stoi(tmp2[0]);
                std::cout << frame << std::endl;
                std::cout << tmp2[1] << std::endl;
                std::cout << tmp2[2] << std::endl;

                if (tmp2[1] == "1" && tmp2[2] == "0")
                {
                    action = true;
                    player = true;
                } else if (tmp2[1] == "0" && tmp2[2] == "1") {
                    action = true;
                    player = false;
                } else {
                    action = false;
                    player = false;
                }

            }
            qBot::macro.push_back({fps, frame, -1, -1, -1, -1, -1, action, player});
        }
    }

    size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
        data->append((char*) ptr, size * nmemb);
        return size * nmemb;
    }

    std::string getFileExt(const std::string& s) {
        size_t i = s.rfind('.', s.length());
        if (i != std::string::npos) {
            return(s.substr(i+1, s.length() - i));
        }
        return("");
    }

    void saveMacro(nfdchar_t* outPath)
    {
        std::ofstream out(outPath, std::ios::binary);
        out.write((char*)qBot::macro.data(),sizeof(Click)*qBot::macro.size());
        GUI::mode = 0;
    }

    void loadMacro(nfdchar_t* outPath)
    {

        std::ifstream in(outPath, std::ios::binary);
        size_t count = in.seekg(0, std::ios::end).tellg()/sizeof(Click);
        qBot::macro.resize(count);
        in.seekg(0);
        in.read((char*)qBot::macro.data(),sizeof(Click)*count);
        GUI::mode = 2;

    }
    namespace LoginWindow
    {
        struct {
            std::string username;
            std::string password;
            bool rememberCredentials;
        } loginStruct;

        struct {
            std::string username;
            std::string password;
            std::string password2;
            std::string licenseKey;
        } registerStruct;

        bool _login = false;
        bool _register = false;


        void loginFunc()
        {
            /* 
            one means everything is fine
            zero means hwids don't match
            negative one means login data is wrong
            */
            std::string loginLink = "http://adarift.atwebpages.com/qbot/api/api.php?a=auth&n=" + loginStruct.username + "&p=" + loginStruct.password;
            CURL *curl;
            CURLcode res;
            std::string response;
            curl = curl_easy_init();
            if (curl)
            {
                curl_easy_setopt(curl, CURLOPT_URL, loginLink.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                res = curl_easy_perform(curl);
                curl_easy_cleanup(curl);
            }
            if (response == "-1")
            {
                ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "Logging in failed. Check your username, password and internet connection."});
                return;
            }
            
            if (response == id)
            {
                ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Logged in successfully."});
                if (loginStruct.rememberCredentials)
                {
                    auto file = fopen(".qbot\\licenseData.dat", "wb");
                    if (file) {
                        fwrite(&loginStruct, sizeof(loginStruct), 1, file);
                        fclose(file);
                    }
                }
                
                tobyadd = true;
                return;
            } else {
                ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "HWID do not match"});
                return;
            }
        }

        void registerFunc()
        {
            std::string registerLink = "http://adarift.atwebpages.com/qbot/api/api.php?a=reg&n=" + registerStruct.username + "&p=" + registerStruct.password + "&ui=" + id + "&k=" + registerStruct.licenseKey;
            CURL *curl;
            CURLcode res;
            std::string response;
            curl = curl_easy_init();
            if (curl)
            {
                curl_easy_setopt(curl, CURLOPT_URL, registerLink.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                res = curl_easy_perform(curl);
                curl_easy_cleanup(curl);
            }
            if (response == "1")
            {
                ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Registred succesfully. Thanks for purchasing qBot!"});
            } else {
                ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "Registration failed. Check your license key and internet connection."});
            }
            return;
        }

        void render()
        {
            ImGui::Begin("qBot login", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

            auto windowSize = ImGui::GetWindowSize();
            ImVec2 btnSize = {windowSize.x - 18, 24.f};

            if (_login)
            {
                ImGui::InputText("Username", &loginStruct.username);
                ImGui::InputText("Password", &loginStruct.password, ImGuiInputTextFlags_Password);
                ImGui::Checkbox("Remember credentials", &loginStruct.rememberCredentials);
                if (ImGui::Button("Login", btnSize))
                {
                    loginFunc();
                }
                if (ImGui::Button("Go back", btnSize))
                {
                    _login = false;
                }
            } else if (_register)
            {
                ImGui::InputText("Username", &registerStruct.username);
                ImGui::InputText("Password", &registerStruct.password, ImGuiInputTextFlags_Password);
                ImGui::InputText("Confirm password", &registerStruct.password2, ImGuiInputTextFlags_Password);
                ImGui::InputText("License key", &registerStruct.licenseKey);
                if (ImGui::Button("Register", btnSize))
                {
                    if (registerStruct.password == registerStruct.password2)
                    {
                        registerFunc();
                    } else {
                        ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "Passwords do not match"});
                    }
                    
                }
                if (ImGui::Button("Go back", btnSize))
                {
                    _register = false;
                }
                
            } else {
                if (ImGui::Button("Login", buttonSize))
                {
                    _login = true;
                    _register = false;
                }
                if (ImGui::Button("Register", buttonSize))
                {
                    _register = true;
                    _login = false;
                }
            }
            ImGui::End();
        }
    }

    namespace MainWindow
    {
        void render()
        {
            ImGui::Begin("qBot", nullptr, ImGuiWindowFlags_NoResize);
            ImGui::SetWindowSize({300,330});
            mainWindowPos = ImGui::GetWindowPos();

            if (ImGui::BeginTabBar("MainTabBar"))
            {
                if (ImGui::BeginTabItem("General"))
                {
                    ImGui::Combo("Mode", &mode, "Disabled\0Record\0Playback");

                    ImGui::Checkbox("Accuracy fix", &qBot::accuracyFixEnabled);

                    ImGui::Separator();

                    ImGui::Text("Frame:");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", qBot::frame);
                    
                    ImGui::Text("XPos:");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%f", qBot::xpos);

                    ImGui::Separator();
                    
                    if (ImGui::Button("Save macro", buttonSize))
                    {
                        nfdchar_t *outPath;
                        nfdresult_t result = NFD_SaveDialogU8(&outPath, filter, 2, ".qbot\\replays", qBot::levelName.c_str());
                        if (result == NFD_OKAY)
                        {
                            saveMacro(outPath);
                            ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Replay successfully saved to %s", outPath});
                        }
                    }
                    
                    if (ImGui::Button("Load macro", buttonSize))
                    {
                        nfdchar_t *outPath;
                        nfdresult_t result = NFD_OpenDialogU8(&outPath, filter, 2, ".qbot\\replays");
                        if (result == NFD_OKAY)
                        {
                            qBot::macro.clear();
                            loadMacro(outPath);
                            FPSBypass::target_fps = qBot::macro[0].fps;
                            FPSBypass::setFPS();
                            ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Replay %s successfully loaded", outPath});
                        }
                    }
                    
                    if (ImGui::Button("Clear macro", buttonSize))
                    {
                        qBot::macro.clear();
                    }

                    if (ImGui::Button("Import/Export macro", buttonSize))
                    {
                        ImGui::OpenPopup("ImportExportPopup");
                    }
                    if (ImGui::BeginPopup("ImportExportPopup"))
                    {
                        if (ImGui::Button("Import from plain text"))
                        {
                            nfdchar_t *outPath;
                            nfdresult_t result = NFD_OpenDialogU8(&outPath, plainTextFilter, 1, ".qbot\\replays");
                            if (result == NFD_OKAY)
                            {
                                std::stringstream ss;
                                std::string line;
                                std::ifstream in(outPath);
                                if (in.is_open())
                                {
                                    while (getline(in, line))
                                    {
                                        ss << line << std::endl;
                                    }
                                }
                                in.close();
                                std::cout << ss.str() << std::endl;
                                convertFromPlainText(ss.str());
                            }
                        }
                        ImGui::Separator();
                        if (ImGui::Button("Export as plain text"))
                        {
                            nfdchar_t *outPath;
                            nfdresult_t result = NFD_SaveDialogU8(&outPath, plainTextFilter, 1, ".qbot\\replays", qBot::levelName.c_str());
                            if (result == NFD_OKAY)
                            {
                                std::ofstream out;
                                out.open(outPath);
                                if (out.is_open())
                                {
                                    out << convertToPlainText(qBot::macro);
                                }
                                out.close();
                                ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Replay successfully exported to %s", outPath});
                            }
                        }

                        ImGui::EndPopup();
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Assist"))
                {
                    ImGui::Text("Speedhack");
                    ImGui::InputFloat("Speed", &Speedhack::target_speed);
                    if (ImGui::Button("Apply speed", buttonSize))
                    {
                        Speedhack::setSpeed();
                    }
                    
                    ImGui::Separator();

                    ImGui::Text("FPS Bypass");
                    ImGui::InputFloat("FPS", &FPSBypass::target_fps, 10);
                    if (ImGui::Button("Apply FPS", buttonSize))
                    {
                        FPSBypass::setFPS();
                    }

                    ImGui::Separator();
                    ImGui::Checkbox("Frame advance", &FrameAdvance::frameAdvanceEnabled);

                    ImGui::Separator();
                    if (ImGui::Button("Macro editor", buttonSize))
                    {
                        macroEditor = true;
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Video"))
                {
                    ImGui::InputInt("Width", &settings.width);
                    ImGui::InputInt("Height", &settings.height);
                    ImGui::InputInt("FPS", &settings.fps);
                    ImGui::InputText("Codec", &settings.codec);
                    ImGui::InputText("Bitrate", &settings.bitrate);
                    ImGui::InputText("Extra args", &settings.extraArgs);
                    ImGui::Checkbox("Include audio", &rec.includeAudio);
                    ImGui::SameLine();
                    if (ImGui::Button("Presets"))
                    {
                        ImGui::OpenPopup("PresetsPopup");                    
                    }
                    if (ImGui::BeginPopup("PresetsPopup"))
                    {
                        if (ImGui::Button("HD (720p)"))
                        {
                            settings.width = 1280;
                            settings.height = 720;
                            ImGui::CloseCurrentPopup();
                        }
                        if (ImGui::Button("Full HD (1080p)"))
                        {
                            settings.width = 1920;
                            settings.height = 1080;
                            ImGui::CloseCurrentPopup();
                        }
                        if (ImGui::Button("Quad HD (1440p)"))
                        {
                            settings.width = 2560;
                            settings.height = 1440;
                            ImGui::CloseCurrentPopup();
                        }
                        if (ImGui::Button("4K (2160p)"))
                        {
                            settings.width = 3840;
                            settings.height = 2160;
                            ImGui::CloseCurrentPopup();
                        }
                        if (ImGui::Button("NVIDIA GPU codec"))
                        {
                            settings.codec = "h264_nvenc";
                            ImGui::CloseCurrentPopup();
                        }
                        if (ImGui::Button("AMD GPU codec"))
                        {
                            settings.codec = "h264_amf";
                            ImGui::CloseCurrentPopup();
                        }
                        if (ImGui::Button("Intel CPU codec"))
                        {
                            settings.codec = "h264_qsv";
                            ImGui::CloseCurrentPopup();
                        }
                        
                        
                        ImGui::EndPopup();
                    }
                    
                    
                    if (!rec.recording)
                    {
                        if (ImGui::Button("Start recording", buttonSize))
                        {
                            if (qBot::inLevel)
                            {
                                nfdchar_t* path = nullptr;
                                nfdfilteritem_t recorderFilter[] = { {"Video file", "mp4"} };
                                if (NFD_SaveDialogU8(&path, recorderFilter, 1, ".qbot\\videos", qBot::levelName.c_str()) == NFD_OKAY)
                                {
                                    rec.lastFrameTime = 0;
                                    rec.start(path);
                                }   
                            } else {
                                ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "Recording can be started only if you're in level"});
                            }
                        }
                    } else {
                        if (ImGui::Button("Stop recording", buttonSize))
                        {
                            rec.stop();
                        }
                    }

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Settings"))
                {
                    ImGui::Checkbox("FPS multiplier", &FPSMultiplier::enabled);
                    ImGui::Checkbox("Ignore user input", &Hooks::PlayLayer::ignoreUserInputEnabled);
                    ImGui::Checkbox("Dual click", &Hooks::PlayLayer::dualClickEnabled);
                    ImGui::Checkbox("Show status", &qBot::showStatusEnabled);
                    ImGui::Checkbox("Fake cheat indicator", &qBot::fakeCheatIndicatorEnabled);
                    ImGui::Checkbox("Checkpoint fix", &qBot::checkpointFixEnabled);
                    
                    ImGui::EndTabItem();

                }

                if (ImGui::BeginTabItem("About"))
                {
                    ImGui::Text("qBot by");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "adarift");

                    ImGui::Text("qBot version:");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "v0.4-beta");

                    ImGui::Text("ImGui version:");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", ImGui::GetVersion());

                    ImGui::Separator();
                    ImGui::Text("Username:");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), LoginWindow::loginStruct.username.c_str());

                    if (ImGui::Button("Log out", buttonSize))
                    {
                        tobyadd = false;
                        remove(".qbot\\licenseData.dat");
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            if (macroEditor)
            {
                ImGui::Begin("Macro editor", nullptr,  macroEditorFlags);

                ImGui::SetWindowSize(ImVec2(300, 330));
                ImGui::SetWindowPos(ImVec2(mainWindowPos.x + 305, mainWindowPos.y));

                ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 285), false);

                ImGui::InputTextMultiline("##editor", &convertedMacro, ImVec2(140, 285));
                ImGui::EndChild();

                ImGui::SameLine();

                ImGui::BeginChild("ChildR", ImVec2(0, 285), false);

                ImGui::Text("Frame offset");

                ImGui::InputInt("##1", &frameOffset);
                if (ImGui::Button("Apply", {135.f, 24.f}))
                {
                    convertedMacro = convertToPlainText(qBot::macro, frameOffset);
                }

                ImGui::Separator();

                if (ImGui::Button("Load", {135.f, 24.f}))
                {
                    convertedMacro = convertToPlainText(qBot::macro);
                }

                if (ImGui::Button("Save", {135.f, 24.f}))
                {
                    convertFromPlainText(convertedMacro);
                    frameOffset = 0;
                }


                ImGui::Separator();

                if (ImGui::Button("Close", {135.f, 24.f}))
                    macroEditor = false;

                ImGui::EndChild();
                ImGui::End();
            }
            ImGui::End();
        }
    } // namespace MainWindow
    
    namespace Notifications
    {
        void render()
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 7.f);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.00f));
            ImGui::RenderNotifications();
            ImGui::PopStyleVar(1); // Don't forget to Pop()
            ImGui::PopStyleColor(1);
        }
    } // namespace Notifications
    

    void initUI()
    {
        auto file = fopen(".qbot\\licenseData.dat", "rb");
        if (file) {
            fseek(file, 0, SEEK_END);
            auto size = ftell(file);

            if (size == sizeof(LoginWindow::loginStruct)) {
                fseek(file, 0, SEEK_SET);
                fread(&LoginWindow::loginStruct, sizeof(LoginWindow::loginStruct), 1, file);
                fclose(file);
            }
        }
        LoginWindow::loginFunc();

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
        style.WindowTitleAlign                  = ImVec2(0.5f,0.5f);
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
        macroEditorFlags |= ImGuiWindowFlags_NoMove;
        macroEditorFlags |= ImGuiWindowFlags_NoResize;
        
        std::cout << "GUI initialized" << std::endl;
    }

    void renderUI()
    {
        GUI::Notifications::render();
        if (IsDebuggerPresent())
        {
            exit(-1);
        }
        
        if (visible)
        {
            /*
            renderDebug();
            ImGui::ShowDemoWindow();
            */
            if (tobyadd)
            {
                GUI::MainWindow::render();
                FPSBypass::setFPS();
            } else {
                GUI::LoginWindow::render();
            }
        }
    }
} // namespace GUI
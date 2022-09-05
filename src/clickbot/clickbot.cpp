#include "clickbot.hpp"
#include <vector>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

namespace Clickbot
{

    bool enabled = false;
    bool initialized = false;
    int p1ClickVolume = 100;
    int p2ClickVolume = 100;

    FMOD::Channel* pushChannel;
    FMOD::Channel* releaseChannel;
    FMOD::Sound* pushSound;
    FMOD::Sound* releaseSound;
    

    std::string pickRandomSound(std::string path)
    {
        std::vector<std::string> sounds;
        for (const auto & entry : fs::directory_iterator(path))
        {
            sounds.push_back(entry.path().string());
        }
        return sounds[rand() % (int)sounds.size()];
    }

    void InitClickbot()
    {
        
    }

    void PushButton(bool player)
    {
        if (enabled)
        {
            auto system = gd::FMODAudioEngine::sharedEngine()->m_pSystem;
            std::string path = pickRandomSound("clicks\\pushes");

            system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &pushSound);
            system->playSound(pushSound, nullptr, false, &pushChannel);
            system->update();
        }
    }

    void ReleaseButton(bool player)
    {
        if (enabled)
        {
            auto system = gd::FMODAudioEngine::sharedEngine()->m_pSystem;
            std::string path = pickRandomSound("clicks\\releases");

            system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &releaseSound);
            system->playSound(releaseSound, nullptr, false, &releaseChannel);
            system->update();
        }
        
    }
    
} // namespace Clickbot

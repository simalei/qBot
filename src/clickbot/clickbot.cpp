#include "clickbot.hpp"
#include <vector>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;
using namespace irrklang;

namespace Clickbot
{

    bool enabled = false;
    int p1ClickVolume = 100;
    int p2ClickVolume = 100;

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
        Clickbot::engine = createIrrKlangDevice();
    }

    void PushButton(bool player)
    {
        if (enabled)
        {
            std::string path = pickRandomSound("clicks\\pushes");
            
            ISound* sound = engine->play2D(path.c_str(), false, true, false);
            sound->setVolume(p1ClickVolume / (float)100);
            sound->setIsPaused(false);
            sound->drop();
            
        }
    }

    void ReleaseButton(bool player)
    {
        if (enabled)
        {
            std::string path = pickRandomSound("clicks\\releases");

            ISound* sound = engine->play2D(path.c_str(), false, true, false);
            sound->setVolume(p1ClickVolume / (float)100);
            sound->setIsPaused(false);
            sound->drop();
        }
        
    }
    
} // namespace Clickbot

#include "recorder.hpp"

namespace Recorder
{
    int width = 1920;
    int height = 1080;
    bool isRecording = false;
    bool frameHasData;
    float afterEndDuration = 3.f;
    float afterEndExtraTime;
    float songStartOffset;
    bool finishedLevel;
    bool includeAudio = true;
    std::vector<uint8_t> currentFrame;
    std::string ffmpegPath = "ffmpeg";
    std::string codec = "", bitrate = "30M", extraArgs = "", extraAudioArgs = "";


    void start(const std::string& path)
    {
        isRecording = true;
        frameHasData = false;
        currentFrame.resize(width * height * 3, 0);
        finishedLevel = false;
        renderer.width = width;
        renderer.height = height;
        renderer.begin();

        auto gameManager = gd::GameManager::sharedState();
        auto playLayer = gameManager->getPlayLayer();
        auto songFile = playLayer->m_level->getAudioFileName();
        auto fadeIn = playLayer->m_pLevelSettings->
    }
}
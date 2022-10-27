#pragma once
#include "../includes.hpp"
#include <thread>
#include <mutex>
#include <CCGL.h>
#include <filesystem>
#include <fstream>
#include <queue>
#include <sstream>
#include "subprocess.hpp"

std::string narrow(const wchar_t* str);
inline auto narrow(const std::wstring& str) { return narrow(str.c_str()); }
std::wstring widen(const char* str);
inline auto widen(const std::string& str) { return widen(str.c_str()); }


class RenderTexture {
public:
    unsigned width, height;
    int oldFbo, oldRbo;
    unsigned fbo;
    CCTexture2D* texture;
    void begin();
    void end();
    void capture(std::mutex& lock, std::vector<uint8_t>& data, volatile bool& lul);
};


class Recorder {
public:
    Recorder();
    static auto& get() {
        static Recorder instance;
        return instance;
    }
    std::vector<uint8_t> currentFrame;
    volatile bool frameHasData;
    std::mutex lock;
    RenderTexture renderer;
    int width, height;
    int fps;
    bool recording = false;
    double lastFrameTime, extraTime;
    bool untilEnd = true;
    std::string codec = "", bitrate = "30M", extraArgs = "", extraAudioArgs = "";
    float afterEndDuration = 3.f;
    float afterEndExtraTime;
    float songStartOffset;
    bool finishedLevel;
    bool includeAudio = true;
    std::string ffmpegPath = "ffmpeg";
    void start(const std::string& path);
    void stop();
    void captureFrame();
    void handleRecording(gd::PlayLayer*, float dt);
    void updateSongOffset(gd::PlayLayer*);
};
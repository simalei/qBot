#pragma once
#include "../includes.hpp"
#include <thread>
#include <mutex>




namespace Recorder
{
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

    extern bool isRecording;
    extern RenderTexture renderer;
    extern int width;
    extern int height;
    extern int fps;
    extern std::string codec, bitrate, extraArgs, extraAudioArgs;
    extern float afterEndDuration;
    extern float afterEndExtraTime;
    extern float songStartOffset;
    extern bool finishedLevel;
    extern bool includeAudio;
    extern std::string ffmpegPath;

    void start(const std::string& path);
    void stop();
    void capture_frame();
    void handle_recording(gd::PlayLayer*, float dt);
    void update_song_offset(gd::PlayLayer*);
}


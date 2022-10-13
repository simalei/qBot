#include "recorder.hpp"
#include <sstream>
#include <CCGL.h>
#include <filesystem>
#include <fstream>


std::string narrow(const wchar_t* str) {
    int size = WideCharToMultiByte(CP_UTF8, 0, str, -1, nullptr, 0, nullptr, nullptr);
    if (size <= 0) { /* fuck */ }
    auto buffer = new char[size];
    WideCharToMultiByte(CP_UTF8, 0, str, -1, buffer, size, nullptr, nullptr);
    std::string result(buffer, size_t(size) - 1);
    delete[] buffer;
    return result;
}

std::wstring widen(const char* str) {
    int size = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (size <= 0) { /* fuck */ }
    auto buffer = new wchar_t[size];
    MultiByteToWideChar(CP_UTF8, 0, str, -1, buffer, size);
    std::wstring result(buffer, size_t(size) - 1);
    delete[] buffer;
    return result;
}

Recorder::Recorder() : width(1280), height(720), fps(60) {}

void Recorder::start(const std::string& path)
{
    recording = true;
    frameHasData = false;
    currentFrame.resize(width * height * 3, 0);
    finishedLevel = false;
    renderer.width = width;
    renderer.height = height;
    renderer.begin();
    auto gameManager = gd::GameManager::sharedState();
    auto playLayer = gameManager->getPlayLayer();
    auto songFile = playLayer->m_level->getAudioFileName();
    auto fadeIn = playLayer->m_pLevelSettings->m_fadeIn;
    auto fadeOut = playLayer->m_pLevelSettings->m_fadeOut;
    auto bgVolume = gameManager->m_fBackgroundMusicVolume;
    auto sfxVolume = gameManager->m_fEffectsVolume;
    if (playLayer->m_level->m_nSongID == 0)
        songFile = CCFileUtils::sharedFileUtils()->fullPathForFilename(songFile.c_str(), false);
    auto isTestmode = playLayer->m_isTestMode;
    auto songOffset = songStartOffset;
    std::thread([&, path, songFile, fadeIn, fadeOut, bgVolume, sfxVolume, isTestmode, songOffset]() {
        std::stringstream ss;
        ss << '"' << ffmpegPath << '"' << " -y -f rawvideo -pix_fmt rgb24 -s " << width << "x" << height << " -r " << fps << " -i - ";
        if (!codec.empty())
            ss << "-c:v " << codec << " ";
        if (!bitrate.empty())
            ss << "-b:v " << bitrate << " ";
        if (!extraArgs.empty())
            ss << extraArgs << " ";
        else
            ss << "-pix_fmt yuv420p ";
        ss << "-vf \"vflip\" -an \"" << path << "\" ";
        auto process = subprocess::Popen(ss.str());
        while (recording || frameHasData)
        {
            lock.lock();
            if (frameHasData)
            {
                const auto frame = currentFrame;
                frameHasData = false;
                lock.unlock();
                process.m_stdin.write(frame.data(), frame.size());
            } else lock.unlock();
        }
        if (process.close())
        {
            std::cout << "smth bad happend" << std::endl;
            return;
        }
        if (!includeAudio || !std::filesystem::exists(songFile))
        {
            return;
        }
        wchar_t buffer[MAX_PATH];
        if (!GetTempFileNameW(widen(std::filesystem::temp_directory_path().string()).c_str(), L"rec", 0, buffer)) {
            return;
        }
        auto temp_path = narrow(buffer) + "." + std::filesystem::path(path).filename().string();
        std::filesystem::rename(buffer, temp_path);
        auto total_time = lastFrameTime;
        {
            std::stringstream stream;
            stream << '"' << ffmpegPath << '"' << " -y -ss " << songOffset << " -i \"" << songFile
            << "\" -i \"" << path << "\" -t " << total_time << " -c:v copy ";
            if (!extraAudioArgs.empty())
                stream << extraAudioArgs << " ";
            stream << "-filter:a \"volume=1[0:a]";
            if (fadeIn && !isTestmode)
                stream << ";[0:a]afade=t=in:d=2[0:a]";
            if (fadeOut && finishedLevel)
                stream << ";[0:a]afade=t=out:d=2:st=" << (total_time - afterEndDuration - 3.5f) << "[0:a]";
            std::cout << "in " << fadeIn << " out " << fadeOut << std::endl;
            stream << "\" \"" << temp_path << "\"";
            std::cout << "executing: " << stream.str() << std::endl;
            auto process = subprocess::Popen(stream.str());
            if (process.close()) {
                return;
            }
        }
    
    std::filesystem::remove(widen(path));
    std::filesystem::rename(temp_path, widen(path));
    std::cout << "video + audio should be done now!" << std::endl;
    
    }).detach();
}
    
void Recorder::stop() {
    renderer.end();
    recording = false;
    
}
void Recorder::updateSongOffset(gd::PlayLayer* playLayer) {
    songStartOffset = playLayer->m_pLevelSettings->m_songStartOffset + playLayer->timeForXPos2(
        playLayer->m_pPlayer1->m_position.x, playLayer->m_isTestMode);
}
void RenderTexture::begin()
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &oldFbo);
    texture = new CCTexture2D;
    {
        auto data = malloc(width * height * 3);
        memset(data, 0, width * height * 3);
        texture->initWithData(data, kCCTexture2DPixelFormat_RGB888, width, height, CCSize(static_cast<float>(width), static_cast<float>(height)));
        free(data);
    }
    glGetIntegerv(GL_RENDERBUFFER_BINDING_EXT, &oldRbo);
    glGetIntegerv(GL_RENDERBUFFER_BINDING_EXT, &oldRbo);
    glGenFramebuffersEXT(1, &fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture->getName(), 0);
    texture->setAliasTexParameters();
    texture->autorelease();
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, oldRbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, oldFbo);
}
void RenderTexture::capture(std::mutex& lock, std::vector<uint8_t>& data, volatile bool& lul)
{
    glViewport(0, 0, width, height);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &oldFbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    auto director = CCDirector::sharedDirector();
    auto scene = director->getRunningScene();
    scene->visit();
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    lock.lock();
    lul = true;
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    lock.unlock();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, oldFbo);
    director->setViewport();
}
void RenderTexture::end() {
    texture->release();
}
void Recorder::captureFrame()
{
    while (frameHasData) {}
    renderer.capture(lock, currentFrame, frameHasData);
}
void Recorder::handleRecording(gd::PlayLayer* play_layer, float dt) {
    if (!play_layer->m_hasLevelCompleteMenu || afterEndExtraTime < afterEndDuration) {
        if (play_layer->m_hasLevelCompleteMenu) {
            afterEndExtraTime += dt;
            finishedLevel = true;
        }
        auto frame_dt = 1. / static_cast<double>(fps);
        auto time = play_layer->m_time + extraTime - lastFrameTime;
        if (time >= frame_dt) {
            gd::FMODAudioEngine::sharedEngine()->setBackgroundMusicTime(
                play_layer->m_time + songStartOffset);
            extraTime = time - frame_dt;
            lastFrameTime = play_layer->m_time;
            captureFrame();
        }
    } else {
        stop();
    }
}

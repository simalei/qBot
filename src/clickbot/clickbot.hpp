#pragma once
#include <includes.hpp>


namespace Clickbot
{
    extern bool enabled;
    extern int p1ClickVolume;
    extern int p2ClickVolume;

    void PushButton(bool player);
    void ReleaseButton(bool player);
    void InitClickbot();
    void Reset();
    void Quit();

    std::string pickRandomSound(std::string path);


} // namespace Clickbot

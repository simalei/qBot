#include "Speedhack.hpp"

namespace Speedhack
{
    float target_speed = 1.0f;
    void setSpeed(float speed)
    {
        CCDirector::sharedDirector()->getScheduler()->setTimeScale(speed);
        FPSBypass::setFPS(speed);
    }
    void setSpeed()
    {
        CCDirector::sharedDirector()->getScheduler()->setTimeScale(target_speed);
        FPSBypass::setFPS(target_speed);
    }
} // namespace Speedhack

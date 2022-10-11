#include "Speedhack.hpp"

namespace Speedhack
{
    float target_speed = 1.0f;
    void setSpeed()
    {
        CCDirector::sharedDirector()->getScheduler()->setTimeScale(target_speed);
    }
} // namespace Speedhack

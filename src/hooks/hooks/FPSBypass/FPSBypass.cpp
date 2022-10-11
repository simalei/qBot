#include "FPSBypass.hpp"

namespace FPSBypass
{
    typedef void*   (__cdecl *fSharedApplication)();
    typedef void    (__thiscall *fSetAnimationInterval)(void *instance, double delay);
    fSharedApplication sharedApplication;
    fSetAnimationInterval setAnimInterval;
    float target_fps = 60;

    void setFPS() {
        float interval = 1.0f / target_fps;
        FPSMultiplier::target_fps = target_fps;
        

        HMODULE hMod = LoadLibrary(LPCSTR("libcocos2d.dll"));
        sharedApplication = (fSharedApplication)GetProcAddress(hMod, "?sharedApplication@CCApplication@cocos2d@@SAPAV12@XZ");
        setAnimInterval = (fSetAnimationInterval)GetProcAddress(hMod, "?setAnimationInterval@CCApplication@cocos2d@@UAEXN@Z");

        void *application = sharedApplication();
        setAnimInterval(application, interval);
    }
} // namespace FPSBypass

#pragma once

#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_HTML5)

namespace dmKaiOS
{

typedef void (*OnKaiOSKeyboardEventCallback)(const bool pressed, const char* keyIdentifier);

extern "C" {
    void KaiOS_SetKeyboardListener(OnKaiOSKeyboardEventCallback callback);
}

}

#endif

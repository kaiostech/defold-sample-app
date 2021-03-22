#pragma once

#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_HTML5)

enum KaiAdsResultCodes {
    PRELOAD_ERROR = 0,
    PRELOAD_OK = 1,
    SHOW_ERROR = 2,
    AD_DISPLAY = 3,
    AD_CLICK = 4,
    AD_CLOSE = 5,
};

typedef void (*OnKaiAdsEventCallback)(const int event, const int code);

extern "C" {
    void KaiAds_PlatformPreload(const char* publisher, const char* config, OnKaiAdsEventCallback callback);
    void KaiAds_PlatformShow(OnKaiAdsEventCallback callback);
}

#endif

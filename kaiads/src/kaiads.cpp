// Extension lib defines
#define EXTENSION_NAME kaiads
#define LIB_NAME "kaiads"
#define MODULE_NAME LIB_NAME

// Defold SDK
#define DLIB_LOG_DOMAIN LIB_NAME
#include <dmsdk/sdk.h>
#include "kaiads.h"


#if defined(DM_PLATFORM_HTML5)


struct KaiAds
{
    KaiAds()
    {
        memset(this, 0, sizeof(*this));
    }

    const char* m_Publisher;
    dmScript::LuaCallbackInfo* m_Listener;
};

static KaiAds g_KaiAds;



static void KaiAds_OnEvent(const int event, const int code)
{
    dmLogInfo("KaiAds_OnEvent %d code: %d", event, code);

    lua_State* L = dmScript::GetCallbackLuaContext(g_KaiAds.m_Listener);
    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(g_KaiAds.m_Listener))
    {
        dmLogError("Unable to setup callback");
        return;
    }

    lua_pushnumber(L, event);
    lua_pushnumber(L, code);

    dmScript::PCall(L, 3, 0); // self + # user arguments

    dmScript::TeardownCallback(g_KaiAds.m_Listener);
}

static int KaiAdsShow(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    dmLogInfo("KaiAdsShow");

    KaiAds_PlatformShow((OnKaiAdsEventCallback)KaiAds_OnEvent);
    return 0;
}

static int KaiAdsPreload(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);

    const char* config = luaL_checkstring(L, 1);
    dmLogInfo("KaiAdsPreload %s", config);

    KaiAds_PlatformPreload(g_KaiAds.m_Publisher, config, (OnKaiAdsEventCallback)KaiAds_OnEvent);
    return 0;
}

static int KaiAdsInit(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    g_KaiAds.m_Publisher = luaL_checkstring(L, 1);
    dmLogInfo("KaiAdsInit %s", g_KaiAds.m_Publisher);
    return 0;
}

static int KaiAdsSetListener(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);

    dmLogInfo("KaiAdsSetListener");

    if (g_KaiAds.m_Listener != 0)
    {
        dmScript::DestroyCallback(g_KaiAds.m_Listener);
        g_KaiAds.m_Listener = 0;
    }

    g_KaiAds.m_Listener = dmScript::CreateCallback(L, 1);
    return 0;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"init", KaiAdsInit},
    {"set_listener", KaiAdsSetListener},
    {"preload", KaiAdsPreload},
    {"show", KaiAdsShow},
    {0, 0}
};

void KaiAds_PushConstants(lua_State* L)
{
    #define SETCONSTANT(name) \
            lua_pushnumber(L, (lua_Number) name); \
            lua_setfield(L, -2, #name);\

        SETCONSTANT(PRELOAD_ERROR)
        SETCONSTANT(PRELOAD_OK)
        SETCONSTANT(SHOW_ERROR)
        SETCONSTANT(AD_DISPLAY)
        SETCONSTANT(AD_CLICK)
        SETCONSTANT(AD_CLOSE)

    #undef SETCONSTANT
}

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    luaL_register(L, MODULE_NAME, Module_methods);

    KaiAds_PushConstants(L);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

static dmExtension::Result AppInitializeKaiAdsExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeKaiAdsExtension(dmExtension::Params* params)
{
    LuaInit(params->m_L);
    dmLogInfo("Registered %s Extension", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeKaiAdsExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeKaiAdsExtension(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

#else

static dmExtension::Result AppInitializeKaiAdsExtension(dmExtension::AppParams* params)
{
    dmLogWarning("Registered %s (null) Extension", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeKaiAdsExtension(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeKaiAdsExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeKaiAdsExtension(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

#endif

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeKaiAdsExtension, AppFinalizeKaiAdsExtension, InitializeKaiAdsExtension, 0, 0, FinalizeKaiAdsExtension)

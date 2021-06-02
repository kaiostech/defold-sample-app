#define LIB_NAME "KaiOS"
#define MODULE_NAME "kaios"

#include <dmsdk/sdk.h>
#include "kaios.h"

#if defined(DM_PLATFORM_HTML5)
#include <emscripten/emscripten.h> // for EM_ASM
#endif

namespace dmKaiOS
{
	#if defined(DM_PLATFORM_HTML5)

	struct InputEvent
	{
		InputEvent() {
			memset(this, 0, sizeof(*this));
		}
		dmHID::Key m_Key;
		bool m_Pressed;
	};


	struct KaiOSContext
	{
		dmHID::HContext         m_HidContext;
		dmArray<InputEvent>		m_KeyboardEvents;
	} g_KaiOS;


	static void KaiOS_AddKeyboardEvent(dmHID::Key key, bool pressed)
	{
		// find event and update if it already exists in the list
		for (uint32_t i = 0; i < g_KaiOS.m_KeyboardEvents.Size(); ++i)
		{
			InputEvent& ev = g_KaiOS.m_KeyboardEvents[i];
			if (ev.m_Key == key)
			{
				ev.m_Pressed = pressed;
				return;
			}
		}

		if (g_KaiOS.m_KeyboardEvents.Full())
		{
			g_KaiOS.m_KeyboardEvents.OffsetCapacity(8);
		}

		InputEvent ev;
		ev.m_Key = key;
		ev.m_Pressed = pressed;
		g_KaiOS.m_KeyboardEvents.Push(ev);
	}

	static dmHID::Key KeyIdentifierToDefoldKey(const char* s)
	{
		if (dmStrCaseCmp("SoftLeft", s) == 0)
		{
			return dmHID::KEY_F1;
		}
		else if (dmStrCaseCmp("SoftRight", s) == 0)
		{
			return dmHID::KEY_F2;
		}
		else if (dmStrCaseCmp("Call", s) == 0)
		{
			return dmHID::KEY_MENU;
		}
		return dmHID::MAX_KEY_COUNT;
	}

	static void KaiOS_OnKeyboardEvent(const bool pressed, const char* keyIdentifier)
	{
		dmHID::Key key = KeyIdentifierToDefoldKey(keyIdentifier);
		if (key != dmHID::MAX_KEY_COUNT)
		{
			KaiOS_AddKeyboardEvent(key, pressed);
		}
	}

	static int KaiOS_PlaySound(lua_State* L)
	{
		DM_LUA_STACK_CHECK(L, 0);

		const char* filepath = luaL_checkstring(L, 1);

		EM_ASM({ defold.playAudioFromURL(Module.UTF8ToString($0)); }, filepath);

		return 0;
	}

	static int KaiOS_Exit(lua_State* L)
	{
		DM_LUA_STACK_CHECK(L, 0);

		EM_ASM(window.close());

		return 0;
	}

	static const luaL_reg Module_methods[] =
	{
		{"exit", KaiOS_Exit},
		{"play_sound", KaiOS_PlaySound},
		{0, 0}
	};

	static void LuaInit(lua_State* L)
	{
		int top = lua_gettop(L);

		luaL_register(L, MODULE_NAME, Module_methods);

		lua_pop(L, 1);
		assert(top == lua_gettop(L));
	}

	#endif // defined(DM_PLATFORM_HTML5)

	static dmExtension::Result AppInitialize(dmExtension::AppParams* params)
	{
		#if defined(DM_PLATFORM_HTML5)
		memset(&g_KaiOS, 0, sizeof(g_KaiOS));
		g_KaiOS.m_HidContext = dmEngine::GetHIDContext(params);
		#endif
		return dmExtension::RESULT_OK;
	}

	static dmExtension::Result Initialize(dmExtension::Params* params)
	{
		#if defined(DM_PLATFORM_HTML5)
		LuaInit(params->m_L);
		KaiOS_SetKeyboardListener((OnKaiOSKeyboardEventCallback)KaiOS_OnKeyboardEvent);
		#else
		printf("Extension %s is not supported\n", MODULE_NAME);
		#endif

		return dmExtension::RESULT_OK;
	}

	static dmExtension::Result AppFinalize(dmExtension::AppParams* params)
	{
		return dmExtension::RESULT_OK;
	}

	static dmExtension::Result OnUpdate(dmExtension::Params* params)
	{
		#if defined(DM_PLATFORM_HTML5)
		dmHID::HKeyboard keyboard = dmHID::GetKeyboard(g_KaiOS.m_HidContext, 0);

		for (uint32_t i = 0; i < g_KaiOS.m_KeyboardEvents.Size(); ++i)
		{
			InputEvent& ev = g_KaiOS.m_KeyboardEvents[i];
			dmHID::SetKey(keyboard, ev.m_Key, ev.m_Pressed);
			if (!ev.m_Pressed)
			{
				g_KaiOS.m_KeyboardEvents.EraseSwap(i);
			}
		}

		#endif
		return dmExtension::RESULT_OK;
	}

	static dmExtension::Result Finalize(dmExtension::Params* params)
	{
		return dmExtension::RESULT_OK;
	}

}


DM_DECLARE_EXTENSION(KaiOS, LIB_NAME, dmKaiOS::AppInitialize, dmKaiOS::AppFinalize, dmKaiOS::Initialize, dmKaiOS::OnUpdate, 0, dmKaiOS::Finalize)

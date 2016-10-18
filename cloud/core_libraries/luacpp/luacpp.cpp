#include "luacpp.h"

int Cloud::Lua::LuaPrint(lua_State* state)
{
    int nargs = lua_gettop(state);
    std::stringstream stream;

    for (int i = 1; i <= nargs; ++i)
    {
        stream << lua_tostring(state, i);
    }

    CL_TRACE_CHANNEL("LUA", stream.str().c_str());

    return 0;
}

int Cloud::Lua::LuaPanic(lua_State* state)
{
    CL_TRACE_CHANNEL("LUA", "PANIC: unprotected error in call to Lua API:\n(%s)\n", lua_tostring(state, -1));
    return 0;  /* return to Lua to abort */
}

Cloud::Lua::StateUniquePtr Cloud::Lua::NewState()
{
    auto* state = luaL_newstate();
    if (state)
    {
        lua_atpanic(state, &LuaPanic);
    }

    return StateUniquePtr(state);
}

Cloud::Lua::StateUniquePtr Cloud::Lua::NewStateAndSetup()
{
    auto luaState = NewState();

    // open standard libs
    auto* s = luaState.get();
    luaL_openlibs(s);


    // redirect print function
    const luaL_Reg printlib[] = {
        { "print", Lua::LuaPrint },
        { nullptr, nullptr }, /* end of array */
    };

    lua_getglobal(s, "_G");
    luaL_setfuncs(s, printlib, 0);
    lua_pop(s, 1);


    return luaState;
}

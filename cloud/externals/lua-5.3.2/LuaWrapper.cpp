#include "LuaWrapper.h"

std::map<void*, size_t> Cloud::Lua::s_pointerTypeRegister;

#ifdef _DEBUG
std::map<size_t, std::string> Cloud::Lua::s_typenames;
#endif

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

Cloud::Lua::ErrorCode Cloud::Lua::LoadFile(lua_State* state, const std::string& filename)
{
    ErrorCode result = static_cast<ErrorCode>(luaL_loadfile(state, filename.c_str()));

    if (result == ErrorCode::ErrFile)
    {
        if (lua_isstring(state, -1))
        {
            const char* err = lua_tostring(state, -1);
            CL_TRACE_CHANNEL("LUA", "Lua file error:\n%s", err);
        }
    }

    if (result == ErrorCode::ErrSyntax)
    {
        if (lua_isstring(state, -1))
        {
            const char* err = lua_tostring(state, -1);
            CL_TRACE_CHANNEL("LUA", "Lua syntax error:\n%s", err);
        }
    }

    return result;
}

Cloud::Lua::ErrorCode Cloud::Lua::DoFile(lua_State* state, const std::string& filename)
{
    ErrorCode result;
    result = LoadFile(state, filename);
    if (result != ErrorCode::Ok)
    {
        return result;
    }

    result = PCall(state);
    return result;
}

Cloud::Lua::ErrorCode Cloud::Lua::PCall(lua_State* state)
{
    ErrorCode result = static_cast<ErrorCode>(lua_pcall(state, 0, LUA_MULTRET, 0));

    if (result == ErrorCode::ErrRun)
    {
        if (lua_isstring(state, -1))
        {
            const char* err = lua_tostring(state, -1);
            CL_TRACE_CHANNEL("LUA", "Lua run error:\n%s", err);
        }
    }

    return result;
}



void Cloud::Lua::Register(lua_State* state, const std::string& funcName, lua_CFunction func)
{
    lua_register(state, funcName.c_str(), func);
}
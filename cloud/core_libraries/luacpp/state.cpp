#include "state.h"

Cloud::LuaState::LuaState()
{
    m_state = Lua::NewStateAndSetup();
}

Cloud::LuaState::LuaState(LuaState&& other)
{
    m_state = std::move(other.m_state);
}

void Cloud::LuaState::Register(const t_char* funcName, lua_CFunction func)
{
    lua_register(GetState(), funcName, func);
}

bool Cloud::LuaState::CheckStack(int requiredStackSlots)
{
    return lua_checkstack(GetState(), requiredStackSlots) != 0 ? true : false;
}

int Cloud::LuaState::GetTop() const
{
    return lua_gettop(GetState());
}

void Cloud::LuaState::SetTop(int stackIndex)
{
    lua_settop(GetState(), stackIndex);
}

Cloud::Lua::Type Cloud::LuaState::Type(int stackIndex) const
{
    return static_cast<Lua::Type>(lua_type(GetState(), stackIndex));
}

const t_char* Cloud::LuaState::Typename(int stackIndex) const
{
    return luaL_typename(GetState(), stackIndex);
}

void Cloud::LuaState::Push(bool value)
{
    lua_pushboolean(GetState(), value ? 1 : 0);
}

void Cloud::LuaState::Push(int value)
{
    lua_pushinteger(GetState(), static_cast<lua_Integer>(value));
}

void Cloud::LuaState::Push(float value)
{
    lua_pushnumber(GetState(), static_cast<lua_Number>(value));
}

const t_char* Cloud::LuaState::Push(const t_char* value)
{
    return lua_pushstring(GetState(), value);
}

void Cloud::LuaState::Pop(int numElements)
{
    lua_pop(GetState(), numElements);
}

void Cloud::LuaState::Remove(int stackIndex)
{
    lua_remove(GetState(), stackIndex);
}

void Cloud::LuaState::PushValue(int sourceIndex)
{
    lua_pushvalue(GetState(), sourceIndex);
}

void Cloud::LuaState::Insert(int targetIndex)
{
    lua_insert(GetState(), targetIndex);
}

void Cloud::LuaState::Replace(int targetIndex)
{
    lua_replace(GetState(), targetIndex);
}

void Cloud::LuaState::CreateTable(int narr, int nrec)
{
    lua_createtable(GetState(), narr, nrec);
}

Cloud::Lua::Type Cloud::LuaState::GetTable(int stackIndex)
{
    return static_cast<Lua::Type>(GetState(), stackIndex);
}

void Cloud::LuaState::SetTable(int stackIndex)
{
    // todo: check if table
    lua_settable(GetState(), stackIndex);
}

Cloud::Lua::Type Cloud::LuaState::GetField(int stackIndex, const t_char* key)
{
    return static_cast<Lua::Type>(lua_getfield(GetState(), stackIndex, key));
}

void Cloud::LuaState::SetField(int targetIndex, const t_char* key)
{
    // todo: check if table
    lua_setfield(GetState(), targetIndex, key);
}

bool Cloud::LuaState::GetMetatable(int stackIndex)
{
    return lua_getmetatable(GetState(), stackIndex) != 0 ? true : false;
}

void Cloud::LuaState::SetMetatable(int stackIndex)
{
    lua_setmetatable(GetState(), stackIndex);
}

Cloud::Lua::Type Cloud::LuaState::GetGlobal(const t_char* name)
{
    return static_cast<Lua::Type>(lua_getglobal(GetState(), name));
}

void Cloud::LuaState::SetGlobal(const t_char* name)
{
    lua_setglobal(GetState(), name);
}

Cloud::Lua::Type Cloud::LuaState::RawGet(int stackIndex)
{
    return static_cast<Lua::Type>(lua_rawget(GetState(), stackIndex));
}

void Cloud::LuaState::RawSet(int stackIndex)
{
    lua_rawset(GetState(), stackIndex);
}

Cloud::Lua::Type Cloud::LuaState::RawGetI(int stackIndex, int tableIndex)
{
    return static_cast<Lua::Type>(lua_rawgeti(GetState(), stackIndex, tableIndex));
}

void Cloud::LuaState::RawSetI(int stackIndex, int tableIndex)
{
    lua_rawseti(GetState(), stackIndex, tableIndex);
}

size_t Cloud::LuaState::RawLen(int stackIndex) const
{
    return lua_rawlen(GetState(), stackIndex);
}

bool Cloud::LuaState::Next(int stackIndex)
{
    return lua_next(GetState(), stackIndex) != 0 ? true : false;
}

Cloud::Lua::ErrorCode Cloud::LuaState::LoadFile(const t_char* fileName)
{
    Lua::ErrorCode result = static_cast<Lua::ErrorCode>(luaL_loadfile(GetState(), fileName));

    if (result == Lua::ErrorCode::ErrFile)
    {
        if (lua_isstring(GetState(), -1))
        {
            const char* err = lua_tostring(GetState(), -1);
            CL_TRACE_CHANNEL("LUA", "Lua file error:\n%s", err);
        }
    }

    if (result == Lua::ErrorCode::ErrSyntax)
    {
        if (lua_isstring(GetState(), -1))
        {
            const char* err = lua_tostring(GetState(), -1);
            CL_TRACE_CHANNEL("LUA", "Lua syntax error:\n%s", err);
        }
    }

    return result;
}

Cloud::Lua::ErrorCode Cloud::LuaState::DoFile(const t_char* fileName)
{
    Lua::ErrorCode result;
    result = LoadFile(fileName);
    if (result != Lua::ErrorCode::Ok)
    {
        return result;
    }

    result = PCall();
    return result;
}

Cloud::Lua::ErrorCode Cloud::LuaState::PCall(int argCount, int retArgCount)
{
    Lua::ErrorCode result = static_cast<Lua::ErrorCode>(lua_pcall(GetState(), argCount, retArgCount, 0));

    if (result == Lua::ErrorCode::ErrRun)
    {
        if (lua_isstring(GetState(), -1))
        {
            const char* err = lua_tostring(GetState(), -1);
            CL_TRACE_CHANNEL("LUA", "Lua run error:\n%s", err);
        }
    }

    return result;
}
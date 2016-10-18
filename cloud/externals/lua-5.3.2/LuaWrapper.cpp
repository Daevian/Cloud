#include "LuaWrapper.h"

std::unordered_map<void*, size_t> Cloud::LuaStateEx::s_pointerTypeRegister;

#ifdef _DEBUG
std::unordered_map<size_t, std::string> Cloud::LuaStateEx::s_typenames;
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

Cloud::LuaState::LuaState()
{
    m_state = Lua::NewStateAndSetup();
}

Cloud::LuaState::LuaState(LuaState&& other)
{
    m_state = std::move(other.m_state);
}

void Cloud::LuaState::Register(const CLchar* funcName, lua_CFunction func)
{
    lua_register(GetState(), funcName, func);
}

CLbool Cloud::LuaState::CheckStack(CLint requiredStackSlots)
{
    return lua_checkstack(GetState(), requiredStackSlots) != 0 ? true : false;
}

CLint Cloud::LuaState::GetTop() const
{
    return lua_gettop(GetState());
}

void Cloud::LuaState::SetTop(CLint stackIndex)
{
    lua_settop(GetState(), stackIndex);
}

Cloud::Lua::Type Cloud::LuaState::Type(CLint stackIndex) const
{
    return static_cast<Lua::Type>(lua_type(GetState(), stackIndex));
}

const CLchar* Cloud::LuaState::Typename(CLint stackIndex) const
{
    return luaL_typename(GetState(), stackIndex);
}

void Cloud::LuaState::Push(CLbool value)
{
    lua_pushboolean(GetState(), value ? 1 : 0);
}

void Cloud::LuaState::Push(CLint value)
{
    lua_pushinteger(GetState(), static_cast<lua_Integer>(value));
}

void Cloud::LuaState::Push(CLfloat value)
{
    lua_pushnumber(GetState(), static_cast<lua_Number>(value));
}

const CLchar* Cloud::LuaState::Push(const CLchar* value)
{
    return lua_pushstring(GetState(), value);
}

void Cloud::LuaState::Pop(CLint numElements)
{
    lua_pop(GetState(), numElements);
}

void Cloud::LuaState::Remove(CLint stackIndex)
{
    lua_remove(GetState(), stackIndex);
}

void Cloud::LuaState::PushValue(CLint sourceIndex)
{
    lua_pushvalue(GetState(), sourceIndex);
}

void Cloud::LuaState::Insert(CLint targetIndex)
{
    lua_insert(GetState(), targetIndex);
}

void Cloud::LuaState::Replace(CLint targetIndex)
{
    lua_replace(GetState(), targetIndex);
}

void Cloud::LuaState::CreateTable(CLint narr, CLint nrec)
{
    lua_createtable(GetState(), narr, nrec);
}

Cloud::Lua::Type Cloud::LuaState::GetTable(CLint stackIndex)
{
    return static_cast<Lua::Type>(GetState(), stackIndex);
}

void Cloud::LuaState::SetTable(CLint stackIndex)
{
    // todo: check if table
    lua_settable(GetState(), stackIndex);
}

Cloud::Lua::Type Cloud::LuaState::GetField(CLint stackIndex, const CLchar* key)
{
    return static_cast<Lua::Type>(lua_getfield(GetState(), stackIndex, key));
}

void Cloud::LuaState::SetField(CLint targetIndex, const CLchar* key)
{
    // todo: check if table
    lua_setfield(GetState(), targetIndex, key);
}

CLbool Cloud::LuaState::GetMetatable(CLint stackIndex)
{
    return lua_getmetatable(GetState(), stackIndex) != 0 ? true : false;
}

void Cloud::LuaState::SetMetatable(CLint stackIndex)
{
    lua_setmetatable(GetState(), stackIndex);
}

Cloud::Lua::Type Cloud::LuaState::GetGlobal(const CLchar* name)
{
    return static_cast<Lua::Type>(lua_getglobal(GetState(), name));
}

void Cloud::LuaState::SetGlobal(const CLchar* name)
{
    lua_setglobal(GetState(), name);
}

Cloud::Lua::Type Cloud::LuaState::RawGet(CLint stackIndex)
{
    return static_cast<Lua::Type>(lua_rawget(GetState(), stackIndex));
}

void Cloud::LuaState::RawSet(CLint stackIndex)
{
    lua_rawset(GetState(), stackIndex);
}

Cloud::Lua::Type Cloud::LuaState::RawGetI(CLint stackIndex, CLint tableIndex)
{
    return static_cast<Lua::Type>(lua_rawgeti(GetState(), stackIndex, tableIndex));
}

void Cloud::LuaState::RawSetI(CLint stackIndex, CLint tableIndex)
{
    lua_rawseti(GetState(), stackIndex, tableIndex);
}

CLsize_t Cloud::LuaState::RawLen(CLint stackIndex) const
{
    return lua_rawlen(GetState(), stackIndex);
}

CLbool Cloud::LuaState::Next(CLint stackIndex)
{
    return lua_next(GetState(), stackIndex) != 0 ? true : false;
}

Cloud::Lua::ErrorCode Cloud::LuaState::LoadFile(const CLchar* fileName)
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

Cloud::Lua::ErrorCode Cloud::LuaState::DoFile(const CLchar* fileName)
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

Cloud::Lua::ErrorCode Cloud::LuaState::PCall(CLint argCount, CLint retArgCount)
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

Cloud::LuaStackSentry::LuaStackSentry(const LuaState& state)
    : m_state(state)
    , m_stackSize(state.GetTop())
{
}

Cloud::LuaStackSentry::~LuaStackSentry()
{
    auto top = m_state.GetTop();
    CL_ASSERT(m_stackSize == top, "The lua stack hasn't return to it's original state. Expected size: %d, actual size: %d");//, m_state.GetTop(), m_stackSize);
}
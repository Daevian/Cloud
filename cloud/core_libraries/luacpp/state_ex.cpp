#include "state_ex.h"

Cloud::LuaStateEx::LuaStateEx()
    : LuaState()
{
}

Cloud::LuaStateEx::LuaStateEx(LuaStateEx&& other)
    : LuaState(std::forward<LuaStateEx>(other))
{
    m_functions = std::move(other.m_functions);
}

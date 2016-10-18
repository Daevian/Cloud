#ifndef CLOUD_LUA_CPP_STATE_INLINE
#define CLOUD_LUA_CPP_STATE_INLINE

namespace Cloud
{
    template <>
    inline CLbool Cloud::LuaState::To(CLint stackIndex) const
    {
        return lua_toboolean(GetState(), stackIndex) != 0 ? true : false;
    }

    template <>
    inline CLint Cloud::LuaState::To(CLint stackIndex) const
    {
        return static_cast<CLint>(lua_tointeger(GetState(), stackIndex));
    }

    template <>
    inline CLfloat Cloud::LuaState::To(CLint stackIndex) const
    {
        return static_cast<CLfloat>(lua_tonumber(GetState(), stackIndex));
    }

    template <>
    inline const CLchar* Cloud::LuaState::To(CLint stackIndex) const
    {
        // TODO: might change stack value to str, so not const or thread-safe
        return lua_tostring(GetState(), stackIndex);
    }
}

#endif // CLOUD_LUA_CPP_STATE_INLINE
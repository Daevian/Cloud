#ifndef CLOUD_LUA_CPP_STATE_INLINE
#define CLOUD_LUA_CPP_STATE_INLINE

namespace Cloud
{
    template <>
    inline bool Cloud::LuaState::To(int stackIndex) const
    {
        return lua_toboolean(GetState(), stackIndex) != 0 ? true : false;
    }

    template <>
    inline int Cloud::LuaState::To(int stackIndex) const
    {
        return static_cast<int>(lua_tointeger(GetState(), stackIndex));
    }

    template <>
    inline float Cloud::LuaState::To(int stackIndex) const
    {
        return static_cast<float>(lua_tonumber(GetState(), stackIndex));
    }

    template <>
    inline const t_char* Cloud::LuaState::To(int stackIndex) const
    {
        // TODO: might change stack value to str, so not const or thread-safe
        return lua_tostring(GetState(), stackIndex);
    }
}

#endif // CLOUD_LUA_CPP_STATE_INLINE
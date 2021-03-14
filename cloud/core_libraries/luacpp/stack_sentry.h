#include "../utilities/DefinesTypes.h"
#ifndef CLOUD_LUA_CPP_STACK_SENTRY_HEADER
#define CLOUD_LUA_CPP_STACK_SENTRY_HEADER

#include "../../core_libraries/utilities/DefinesTypes.h"

namespace Cloud
{
    class LuaState;

    class LuaStackSentry
    {
    public:
        LuaStackSentry(const LuaState& state);
        LuaStackSentry(const LuaStackSentry&) = delete;
        LuaStackSentry(LuaStackSentry&&) = delete;
        ~LuaStackSentry();

    private:
        const LuaState& m_state;
        const int m_stackSize;
    };
}

#endif // CLOUD_LUA_CPP_STACK_SENTRY_HEADER
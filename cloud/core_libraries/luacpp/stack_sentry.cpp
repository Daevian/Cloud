#include "stack_sentry.h"
#include "state.h"

Cloud::LuaStackSentry::LuaStackSentry(const LuaState& state)
    : m_state(state)
    , m_stackSize(state.GetTop())
{
}

Cloud::LuaStackSentry::~LuaStackSentry()
{
    auto top = m_state.GetTop();
    CL_UNUSED(top);
    CL_ASSERT(m_stackSize == top, "The lua stack hasn't return to it's original state. Expected size: %d, actual size: %d");//, m_state.GetTop(), m_stackSize);
}
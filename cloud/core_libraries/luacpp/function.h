#ifndef CLOUD_LUA_CPP_FUNCTION_HEADER
#define CLOUD_LUA_CPP_FUNCTION_HEADER

#include "state.h"

namespace Cloud
{
    class LuaFunctionBase
    {
    public:
        LuaFunctionBase() {};
        LuaFunctionBase(const LuaFunctionBase&) = delete;
        virtual ~LuaFunctionBase() {};
        virtual int Invoke(LuaState& state) = 0;
    };

    template <typename T_Return, typename... T_Args>
    class LuaFunction : public LuaFunctionBase
    {
        using Func = std::function<T_Return(T_Args...)>;
        using ArgsTuple = std::tuple<T_Args...>;

    public:
        LuaFunction(LuaState& state, const t_char* funcName, const Func& func)
            : LuaFunctionBase()
            , m_state(state)
            , m_funcName(funcName)
            , m_function(func)
        {
            m_state.PushLightUserData(this);
            m_state.PushCClosure(LuaFunction::InvokeBase, 1);
            m_state.SetGlobal(funcName);
        }

        LuaFunction(const LuaFunction&) = delete;

        LuaFunction(LuaFunction&& other)
            : m_state(other.m_state)
            , m_funcName(other.m_funcName)
            , m_function(std::move(other.m_function))
        {}

        ~LuaFunction() override
        {
            m_state.PushNil();
            m_state.SetGlobal(m_funcName);
        }

        template <typename... T_Types, std::size_t... N>
        static auto GetArgs(LuaState& state, const int stackIndex, Indices<N...>)
        {
            CL_UNUSED(state);
            CL_UNUSED(stackIndex);
            return std::make_tuple(state.To<T_Types>(stackIndex + N)...);
        }

        template <typename... T_Types>
        static auto GetArgs(LuaState& state, const int stackIndex)
        {
            constexpr auto numArgs = sizeof...(T_Types);
            return GetArgs<T_Types...>(state, stackIndex, IndicesBuilderType<numArgs>());
        }

        template <typename... T_Args, std::size_t... N>
        static auto CallFunction(Func&& func, ArgsTuple&& args, Indices<N...>)
        {
            CL_UNUSED(args);
            return std::forward<Func>(func)(std::get<N>(std::forward<ArgsTuple>(args))...);
        }

        static auto CallFunction(Func&& func, ArgsTuple&& args)
        {
            return CallFunction(std::forward<Func>(func), std::forward<ArgsTuple>(args), IndicesBuilderType<sizeof...(T_Args)>());
        }

        template<typename T_Return, std::size_t T_NumArgs, typename... T_Args>
        struct Invoker
        {
            static int Apply(LuaState& state, Func&& func)
            {
                constexpr auto numArgs = static_cast<int>(sizeof...(T_Args));
                auto&& args = GetArgs<T_Args...>(state, -numArgs);
                auto&& ret = CallFunction(std::forward<Func>(func), std::forward<ArgsTuple>(args));
                state.Push(ret);
                return 1;
            }
        };

        template<std::size_t T_NumArgs, typename... T_Args>
        struct Invoker<void, T_NumArgs, T_Args...>
        {
            static int Apply(LuaState& state, Func&& func)
            {
                constexpr auto numArgs = static_cast<int>(sizeof...(T_Args));
                auto&& args = GetArgs<T_Args...>(state, -numArgs);
                CallFunction(std::forward<Func>(func), std::forward<ArgsTuple>(args));
                return 0;
            }
        };

        int Invoke(LuaState& state) override
        {
            return Invoker<T_Return, sizeof...(T_Args), T_Args...>::Apply(state, std::forward<Func>(m_function));
        }

        static int InvokeBase(lua_State* state)
        {
            auto* func = static_cast<LuaFunction<T_Return, T_Args...>*>(lua_touserdata(state, lua_upvalueindex(1)));
            return func->Invoke(func->m_state);
        }

    private:
        LuaState& m_state;
        const t_char* m_funcName;
        Func m_function;
    };
}

#endif // CLOUD_LUA_CPP_FUNCTION_HEADER

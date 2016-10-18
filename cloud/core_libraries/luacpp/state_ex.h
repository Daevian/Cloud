#ifndef CLOUD_LUA_CPP_STATE_EX_HEADER
#define CLOUD_LUA_CPP_STATE_EX_HEADER

#include "function.h"
#include "stack_sentry.h"

namespace Cloud
{
    class LuaStateEx : public LuaState
    {
    public:
        LuaStateEx();
        LuaStateEx(const LuaStateEx&) = delete;
        LuaStateEx(LuaStateEx&& other);
        virtual ~LuaStateEx() override {};

        template <typename T_Return, typename... T_Args>
        void RegisterFunction(const CLchar* funcName, const std::function<T_Return(T_Args...)>& func)
        {
            LuaStackSentry sentry(*this);

            auto luaFunc = std::make_unique<LuaFunction<T_Return, T_Args...>>(*this, funcName, func);
            m_functions[funcName] = std::move(luaFunc);
        }

        template <typename T_Return>
        void RegisterFunction(const CLchar* funcName, const std::function<T_Return()>& func)
        {
            LuaStackSentry sentry(*this);

            auto luaFunc = std::make_unique<LuaFunction<T_Return>>(*this, funcName, func);
            m_functions[funcName] = std::move(luaFunc);
        }

        template <size_t, typename... T_Types>
        struct ReadbackTypeTrait
        {
            template <typename T>
            static auto Readback(const LuaStateEx& state, const CLint stackIndex)
            {
                auto value = std::make_tuple(state.To<T>(stackIndex));
                return value;
            }

            template <typename T0, typename T1, typename... T_Rest>
            static auto Readback(const LuaStateEx& state, const CLint stackIndex)
            {
                auto first = std::make_tuple(state.To<T0>(stackIndex));
                return std::tuple_cat(first, Readback<T1, T_Rest...>(state, stackIndex + 1));
            }

            static auto Apply(LuaStateEx& state)
            {
                constexpr CLint elementCount = static_cast<CLint>(sizeof...(T_Types));
                auto value = Readback<T_Types...>(state, -elementCount);
                state.Pop(sizeof...(T_Types));
                return value;
            }
        };

        template <typename T>
        struct ReadbackTypeTrait<1, T>
        {
            static auto Apply(LuaStateEx& state)
            {
                auto value = state.To<T>(-1);
                state.Pop(1);
                return value;
            }
        };

        template <typename... T_Types>
        struct ReadbackTypeTrait<0, T_Types...>
        {
            static void Apply(LuaStateEx&) {}
        };

        template <typename... T_Types>
        auto PopReturn()
        {
            return ReadbackTypeTrait<sizeof...(T_Types), T_Types...>::Apply(*this);
        }

        template<typename... ReturnArgs, typename... Args>
        auto Call(const CLchar* functionName, Args&&... args)
        {
            LuaStackSentry sentry(*this);

            const auto type = GetGlobal(functionName);
            if (type != Lua::Type::Function)
            {
                // type error
                //return Lua::ErrorCode::ErrErr;
            }

            // check stack size
            Push(std::forward<Args>(args)...);

            constexpr auto argCount = sizeof...(Args);
            constexpr auto retCount = sizeof...(ReturnArgs);
            auto error = PCall(argCount, retCount);
            CL_UNUSED(error);

            return PopReturn<ReturnArgs...>();

        }

        void ForEach()
        {
            // push nil     [..., {a, b, c, ...}, nil]
            // next -2      [..., {a, b, c, ...}, 1, a]
            // call key, val
            // pop 1        [..., {a, b, c, ...}, 1]
            // next -2      [..., {a, b, c, ...}, 2, b]
            // ...

            // ORDER NOT GUARATEED!
        }

        template <class TYPE>
        static void PushLightUserDataChecked(TYPE* pointer)
        {
            auto typeId = getUniqueTypeId<TYPE>();
            // lock
            s_pointerTypeRegister[pointer] = typeId;
            //unlock

            PushLightUserData(pointer);
        }

        template <class TYPE>
        static TYPE* ToUserDataChecked(CLint index)
        {
            const auto castTypeId = getUniqueTypeId<TYPE>();

            auto&& pointer = ToUserData<TYPE>(index);

            // lock
            const auto registeredTypeId = s_pointerTypeRegister[pointer];
            // unlock

            if (registeredTypeId == castTypeId)
            {
                return pointer;
            }
            else
            {
                // lock
#ifdef _DEBUG
                const char* registeredTypeName = s_typenames[registeredTypeId].c_str();
#else
                const char* registeredTypeName = "<type not available>";
#endif
                CL_TRACE_CHANNEL("LUA",
                                 "Lua ToUserData cast error:\n"
                                 "Trying to cast stack index %d, ptr:%p '%s (typeid:%zu)' to '%s (typeid:%zu)'\n"
                                 "ToUserData will return nullptr",
                                 index, pointer, registeredTypeName, s_pointerTypeRegister[pointer], typeid(TYPE).name(), castTypeId);
                // unlock
                return nullptr;
            }
        }

    private:
        static size_t NextTypeId(const char* name)
        {
            static size_t id = 0;
            size_t result = id;
            // lock
            ++id;
            CL_UNUSED(name);
#ifdef _DEBUG

            s_typenames[result] = name;

#endif
            // unlock
            return result;
        }

        template <typename TYPE>
        static size_t getUniqueTypeId()
        {
            static size_t id = NextTypeId(typeid(TYPE).name());
            return id;
        }

        static std::unordered_map<void*, size_t> s_pointerTypeRegister;
#ifdef _DEBUG
        static std::unordered_map<size_t, std::string> s_typenames;
#endif

        std::unordered_map<const char*, std::unique_ptr<LuaFunctionBase>> m_functions;
    };
}

#endif // CLOUD_LUA_CPP_STATE_EX_HEADER

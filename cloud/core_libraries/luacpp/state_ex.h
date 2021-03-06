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

        template <typename _Return, typename... _Args>
        void RegisterFunction(const t_char* funcName, const std::function<_Return(_Args...)>& func)
        {
            LuaStackSentry sentry(*this);

            auto luaFunc = std::make_unique<LuaFunction<_Return, _Args...>>(*this, funcName, func);
            m_functions[funcName] = std::move(luaFunc);
        }

        template <typename _Return>
        void RegisterFunction(const t_char* funcName, const std::function<_Return()>& func)
        {
            LuaStackSentry sentry(*this);

            auto luaFunc = std::make_unique<LuaFunction<_Return>>(*this, funcName, func);
            m_functions[funcName] = std::move(luaFunc);
        }

        template <size_t, typename... _Types>
        struct ReadbackTypeTrait
        {
            template <typename _T>
            static auto Readback(const LuaStateEx& state, const int stackIndex)
            {
                auto value = std::make_tuple(state.To<_T>(stackIndex));
                return value;
            }

            template <typename _T0, typename _T1, typename... _Rest>
            static auto Readback(const LuaStateEx& state, const int stackIndex)
            {
                auto first = std::make_tuple(state.To<_T0>(stackIndex));
                return std::tuple_cat(first, Readback<_T1, _Rest...>(state, stackIndex + 1));
            }

            static auto Apply(LuaStateEx& state)
            {
                constexpr int elementCount = static_cast<int>(sizeof...(_Types));
                auto value = Readback<_Types...>(state, -elementCount);
                state.Pop(sizeof...(_Types));
                return value;
            }
        };

        template <typename _T>
        struct ReadbackTypeTrait<1, _T>
        {
            static auto Apply(LuaStateEx& state)
            {
                auto value = state.To<_T>(-1);
                state.Pop(1);
                return value;
            }
        };

        template <typename... _Types>
        struct ReadbackTypeTrait<0, _Types...>
        {
            static void Apply(LuaStateEx&) {}
        };

        template <typename... _Types>
        auto PopReturn()
        {
            return ReadbackTypeTrait<sizeof...(_Types), _Types...>::Apply(*this);
        }

        template<typename... _ReturnArgs, typename... _Args>
        auto Call(const t_char* functionName, _Args&&... args)
        {
            LuaStackSentry sentry(*this);

            const auto type = GetGlobal(functionName);
            if (type != Lua::Type::Function)
            {
                // type error
                //return Lua::ErrorCode::ErrErr;
            }

            // check stack size
            Push(std::forward<_Args>(args)...);

            constexpr auto argCount = sizeof...(_Args);
            constexpr auto retCount = sizeof...(_ReturnArgs);
            auto error = PCall(argCount, retCount);
            CL_UNUSED(error);

            return PopReturn<_ReturnArgs...>();

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
        static TYPE* ToUserDataChecked(int index)
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

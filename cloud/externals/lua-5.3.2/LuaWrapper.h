#ifndef CLOUD_LUA_WRAPPER_HEADER
#define CLOUD_LUA_WRAPPER_HEADER



//extern "C"
//{
//
//
////#define lua_writestring(s,l)        (LuaWriteString((s), (l)))
////#define lua_writeline()             (LuaWriteLine())
////#define lua_writestringerror(s,p)   (LuaWriteStringError(s, p))
//
////int LuaWriteString(const char* str, int length)
////{
////    //string;
////    length;
////    printf(str);
////    return 0;
////}
//
////int LuaWriteLine();
//////{
//////    return 0;
//////}
////
////int LuaWriteStringError(const char* string...);
////{
////    //(fprintf(stderr, (s), (p)), fflush(stderr))
////    string;
////    return 0;
////}
//
//
//
//
////#define lua_lock(L)	    (LuaLock)
////#define lua_unlock(L)	(LuaUnlock)
////
////int LuaLock();
////int LuaUnlock();
//
////#define LUA_USER_H "LuaWrapper.h"
//
//}

#include "src/lua.hpp"

#include <sstream>
#include <memory>
#include <functional>
#include <typeinfo>
#include <unordered_map>
#include "../../core_libraries/debugging/DebugLog.h"
#include "../../core_libraries/utilities/DefinesMacros.h"

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
        const CLint m_stackSize;
    };

    class Lua
    {
    public:

        enum class ErrorCode : int
        {
            Ok          = LUA_OK,
            Yield       = LUA_YIELD,
            ErrRun      = LUA_ERRRUN,
            ErrSyntax   = LUA_ERRSYNTAX,
            ErrMem      = LUA_ERRMEM,
            ErrGcMM     = LUA_ERRGCMM,
            ErrErr      = LUA_ERRERR,
            ErrFile     = LUA_ERRFILE,
        };

        enum class Type : int
        {
            None            = LUA_TNONE,
            Nil             = LUA_TNIL,
            Boolean         = LUA_TBOOLEAN,
            LightUserData   = LUA_TLIGHTUSERDATA,
            Number          = LUA_TNUMBER,
            String          = LUA_TSTRING,
            Table           = LUA_TTABLE,
            Function        = LUA_TFUNCTION,
            UserData        = LUA_TUSERDATA,
            Thread          = LUA_TTHREAD,
        };

        struct Deleter
        {
            void operator()(lua_State* ptr)
            {
                lua_close(ptr);
                ptr = nullptr;
            }
        };

        typedef std::unique_ptr<lua_State, Deleter> StateUniquePtr;

        static int LuaPrint(lua_State* state);
        static int LuaPanic(lua_State* state);
        
        static StateUniquePtr NewState();
        static StateUniquePtr NewStateAndSetup();

        static ErrorCode LoadFile(lua_State* state, const CLchar* fileName);
        static ErrorCode DoFile(lua_State* state, const CLchar* fileName);
        static ErrorCode PCall(lua_State* state, CLint argCount = 0, CLint retArgCount = LUA_MULTRET);

        template <class TYPE>
        static void PushLightUserData(lua_State* state, TYPE* pointer);

        template <class TYPE>
        static TYPE* ToUserData(lua_State* state, int index);

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
    };

    template <std::size_t... T_Indices>
    struct Indices {};

    template <std::size_t N, std::size_t... T_Indices>
    struct IndicesBuilder : IndicesBuilder<N-1, N-1, T_Indices...> {};

    template <std::size_t... T_Indices>
    struct IndicesBuilder<0, T_Indices...>
    {
        using Type = Indices<T_Indices...>;
    };

    template<std::size_t _IndexCount>
    using IndicesBuilderType = typename IndicesBuilder<_IndexCount>::Type;


    class LuaState
    {
    public:
        LuaState();
        LuaState(const LuaState&) = delete;
        LuaState(LuaState&& other);
        ~LuaState() {};

        void Register(const CLchar* funcName, lua_CFunction func);

        CLbool          CheckStack(CLint requiredStackSlots); // TODO: test behaviour
        CLint           GetTop() const;
        void            SetTop(CLint stackIndex);

        Lua::Type       Type(CLint stackIndex) const;
        const CLchar*   Typename(CLint stackIndex) const;

        template <Lua::Type TYPE>
        CLbool          IsType(CLint stackIndex) const
        {
            return Type(stackIndex) == Type;
        }
        
        template <class T>
        T* ToUserData(CLint stackIndex) const
        {
            return static_cast<T*>(lua_touserdata(GetState(), stackIndex));
        }

        template <typename TYPE>
        TYPE            To(CLint stackIndex) const
        {
            return ToUserData<std::remove_pointer_t<TYPE>>(stackIndex);
        }

        CLint UpValueIndex(CLint upValueIndex) const
        {
            return lua_upvalueindex(upValueIndex);
        }

        template <typename TYPE>
        TYPE            Opt(CLint stackIndex, const TYPE& defaultValue) const;

        void            Push() {}
        void            Push(CLbool value);
        void            Push(CLint value);
        void            Push(CLfloat value);
        const CLchar*   Push(const CLchar* value);

        template<typename FirstArg, typename... MoreArgs>
        void            Push(FirstArg&& firstArg, MoreArgs&&... moreArgs)
        {
            Push(std::forward<FirstArg>(firstArg));
            Push(std::forward<MoreArgs>(moreArgs)...);
        }

        void PushNil()
        {
            lua_pushnil(GetState());
        }

        template <class TYPE>
        void PushLightUserData(TYPE* pointer)
        {
            lua_pushlightuserdata(GetState(), pointer);
        }

        void PushCClosure(lua_CFunction func, CLint numUpValues)
        {
            lua_pushcclosure(GetState(), func, numUpValues);
        }

        void            Pop(CLint numElements); // TODO: what happens with negative values?
        void            Remove(CLint stackIndex); // TODO: what happens OOB?

        void            PushValue(CLint sourceIndex); // TODO: what happens when indexing OOB?
        void            Insert(CLint targetIndex); // TODO: test what happens if inserting outside stack
        void            Replace(CLint targetIndex); // TODO: test what happens if inserting outside stack

        void            CreateTable(CLint narr = 0, CLint nrec = 0); // TODO: narr and nrec means? and can they be negative...?

        Lua::Type       GetTable(CLint stackIndex);
        void            SetTable(CLint stackIndex);

        Lua::Type       GetField(CLint stackIndex, const CLchar* key);
        void            SetField(CLint stackIndex, const CLchar* key);

        CLbool          GetMetatable(CLint stackIndex);
        void            SetMetatable(CLint stackIndex);

        Lua::Type       GetGlobal(const CLchar* name);
        void            SetGlobal(const CLchar* name);

        void            PushGlobal() // todo: implement
        {
            // push value
            // set global
        }

        Lua::Type       RawGet(CLint stackIndex);
        void            RawSet(CLint stackIndex);
        Lua::Type       RawGetI(CLint stackIndex, CLint tableIndex);
        void            RawSetI(CLint stackIndex, CLint tableIndex);
        CLsize_t        RawLen(CLint stackIndex) const;

        CLbool          Next(CLint stackIndex);

        Lua::ErrorCode LoadFile(const CLchar* fileName);
        Lua::ErrorCode DoFile(const CLchar* fileName);

    protected:
        lua_State* GetState() const { return m_state.get(); }

    private:
        Lua::StateUniquePtr m_state;

    };

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

    class LuaFunctionBase
    {
    public:
        virtual ~LuaFunctionBase() {};
        virtual CLint Invoke(LuaState& state) = 0;
    };

    template <typename T_Return, typename... T_Args>
    class LuaFunction : public LuaFunctionBase
    {
        using Func = std::function<T_Return(T_Args...)>;
        using ArgsTuple = std::tuple<T_Args...>;

    public:
        LuaFunction(LuaState& state, const CLchar* funcName, const Func& func)
            : m_state(state)
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
        static auto GetArgs(LuaState& state, const CLint stackIndex, Indices<N...>)
        {
            CL_UNUSED(state);
            CL_UNUSED(stackIndex);
            return std::make_tuple(state.To<T_Types>(stackIndex + N)...);
        }

        template <typename... T_Types>
        static auto GetArgs(LuaState& state, const CLint stackIndex)
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
            static CLint Apply(LuaState& state, Func&& func)
            {
                constexpr auto numArgs = static_cast<CLint>(sizeof...(T_Args));
                auto&& args = GetArgs<T_Args...>(state, -numArgs);
                auto&& ret = CallFunction(std::forward<Func>(func), std::forward<ArgsTuple>(args));
                state.Push(ret);
                return 1;
            }
        };

        template<std::size_t T_NumArgs, typename... T_Args>
        struct Invoker<void, T_NumArgs, T_Args...>
        {
            static CLint Apply(LuaState& state, Func&& func)
            {
                constexpr auto numArgs = static_cast<CLint>(sizeof...(T_Args));
                auto&& args = GetArgs<T_Args...>(state, -numArgs);
                CallFunction(std::forward<Func>(func), std::forward<ArgsTuple>(args));
                return 0;
            }
        };

        CLint Invoke(LuaState& state) override
        {
            return Invoker<T_Return, sizeof...(T_Args), T_Args...>::Apply(state, std::forward<Func>(m_function));
        }

        static CLint InvokeBase(lua_State* state)
        {
            auto* func = static_cast<LuaFunction<T_Return, T_Args...>*>(lua_touserdata(state, lua_upvalueindex(1)));
            return func->Invoke(func->m_state);
        }

    private:
        LuaState& m_state;
        const CLchar* m_funcName;
        Func m_function;
    };

    class LuaStateEx : public LuaState
    {
    public:

        template <typename T_Return, typename... T_Args>
        void RegisterFunc(const CLchar* funcName, const std::function<T_Return(T_Args...)>& func)
        {
            LuaStackSentry sentry(*this);

            auto luaFunc = std::make_unique<LuaFunction<T_Return, T_Args...>>(*this, funcName, func);
            m_functions[funcName] = std::move(luaFunc);
        }

        template <typename T_Return>
        void RegisterFunc(const CLchar* funcName, const std::function<T_Return()>& func)
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
            static void Apply(LuaStateEx& ) {}
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
            auto error = Lua::PCall(GetState(), argCount, retCount);
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

    private:
        std::unordered_map<const char*, std::unique_ptr<LuaFunctionBase>> m_functions;
    };

    

    template<class TYPE>
    inline void Lua::PushLightUserData(lua_State* state, TYPE* pointer)
    {
        auto typeId = getUniqueTypeId<TYPE>();
        // lock
        s_pointerTypeRegister[pointer] = typeId;
        //unlock
        
        lua_pushlightuserdata(state, pointer);
    }

    template<class TYPE>
    inline TYPE* Lua::ToUserData(lua_State* state, int index)
    {
        const auto castTypeId = getUniqueTypeId<TYPE>();
        
        void* pointer = lua_touserdata(state, index);
        
        // lock
        const auto registeredTypeId = s_pointerTypeRegister[pointer];
        // unlock

        if (registeredTypeId == castTypeId)
        {
            return static_cast<TYPE*>(pointer);
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
}

#endif // CLOUD_LUA_WRAPPER_HEADER
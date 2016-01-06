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
#include <map>
#include <typeinfo>
#include "../../core_libraries/debugging/DebugLog.h"

namespace Cloud
{
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

        static ErrorCode LoadFile(lua_State* state, const std::string& filename);
        static ErrorCode DoFile(lua_State* state, const std::string& filename);
        static ErrorCode PCall(lua_State* state);
        
        static void Register(lua_State* state, const std::string& funcName, lua_CFunction func);

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

        static std::map<void*, size_t> s_pointerTypeRegister;
#ifdef _DEBUG
        static std::map<size_t, std::string> s_typenames;
#endif
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
#ifndef CLOUD_LUA_CPP_HEADER
#define CLOUD_LUA_CPP_HEADER

#include "utility.h"

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
    namespace Lua
    {
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

        int LuaPrint(lua_State* state);
        int LuaPanic(lua_State* state);
        
        StateUniquePtr NewState();
        StateUniquePtr NewStateAndSetup();
    }

}

#endif // CLOUD_LUA_CPP_HEADER
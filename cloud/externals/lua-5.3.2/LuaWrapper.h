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
#include "../../core_libraries/debugging/DebugLog.h"

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

        struct Deleter
        {
            void operator()(lua_State* ptr)
            {
                lua_close(ptr);
                ptr = nullptr;
            }
        };

        typedef std::unique_ptr<lua_State, Deleter> StateUniquePtr;

        inline int LuaPrint(lua_State* state)
        {
            int nargs = lua_gettop(state);
            std::stringstream stream;

            for (int i = 1; i <= nargs; ++i)
            {
                stream << lua_tostring(state, i);
            }

            CL_TRACE_CHANNEL("LUA", stream.str().c_str());

            return 0;
        }

        inline int LuaPanic(lua_State* L)
        {
            CL_TRACE_CHANNEL("LUA", "PANIC: unprotected error in call to Lua API:\n(%s)\n", lua_tostring(L, -1));
            return 0;  /* return to Lua to abort */
        }
        
        inline StateUniquePtr NewState()
        {
            auto* state = luaL_newstate();
            if (state)
            {
                lua_atpanic(state, &LuaPanic);
            }

            return StateUniquePtr(state);
        }

        inline StateUniquePtr NewStateAndSetup()
        {
            auto luaState = NewState();
            
            // open standard libs
            auto* s = luaState.get();
            luaL_openlibs(s);


            // redirect print function
            const luaL_Reg printlib[] = {
                { "print", LuaPrint },
                { nullptr, nullptr }, /* end of array */
            };

            lua_getglobal(s, "_G");
            luaL_setfuncs(s, printlib, 0);
            lua_pop(s, 1);


            return luaState;
        }

        inline ErrorCode LoadFile(lua_State* state, const std::string& filename)
        {
            ErrorCode result = static_cast<ErrorCode>(luaL_loadfile(state, filename.c_str()));
            
            if (result == ErrorCode::ErrFile)
            {
                if (lua_isstring(state, -1))
                {
                    const char* err = lua_tostring(state, -1);
                    CL_TRACE_CHANNEL("LUA", "Lua file error:\n%s", err);
                }
            }

            if (result == ErrorCode::ErrSyntax)
            {
                if (lua_isstring(state, -1))
                {
                    const char* err = lua_tostring(state, -1);
                    CL_TRACE_CHANNEL("LUA", "Lua syntax error:\n%s", err);
                }
            }

            return result;
        }

        inline ErrorCode PCall(lua_State* state)
        {
            ErrorCode result = static_cast<ErrorCode>(lua_pcall(state, 0, LUA_MULTRET, 0));

            if (result == ErrorCode::ErrRun)
            {
                if (lua_isstring(state, -1))
                {
                    const char* err = lua_tostring(state, -1);
                    CL_TRACE_CHANNEL("LUA", "Lua run error:\n%s", err);
                }
            }

            return result;
        }

        inline ErrorCode DoFile(lua_State* state, const std::string& filename)
        {
            ErrorCode result;
            result = LoadFile(state, filename);
            if (result != ErrorCode::Ok)
            {
                return result;
            }

            result = PCall(state);
            return result;
        }

        inline void Register(lua_State* state, const std::string& funcName, lua_CFunction func)
        {
            lua_register(state, funcName.c_str(), func);
        }
    }
}

#endif // CLOUD_LUA_WRAPPER_HEADER
#ifndef CLOUD_LUA_WRAPPER_HEADER
#define CLOUD_LUA_WRAPPER_HEADER



extern "C"
{


#define lua_writestring(s,l)        (LuaWriteString((s), (l)))
//#define lua_writeline()             (LuaWriteLine())
//#define lua_writestringerror(s,p)   (LuaWriteStringError(s, p))

int LuaWriteString(const char* str, int length)
{
    //string;
    length;
    printf(str);
    return 0;
}

//int LuaWriteLine();
////{
////    return 0;
////}
//
//int LuaWriteStringError(const char* string...);
//{
//    //(fprintf(stderr, (s), (p)), fflush(stderr))
//    string;
//    return 0;
//}




//#define lua_lock(L)	    (LuaLock)
//#define lua_unlock(L)	(LuaUnlock)
//
//int LuaLock();
//int LuaUnlock();

#define LUA_USER_H "LuaWrapper.h"

#include "src/lua.hpp"

}

#endif // CLOUD_LUA_WRAPPER_HEADER
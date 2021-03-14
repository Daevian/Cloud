#ifndef CLOUD_LUA_CPP_STATE_HEADER
#define CLOUD_LUA_CPP_STATE_HEADER

#include "luacpp.h"

namespace Cloud
{
    class LuaState
    {
    public:
        LuaState();
        LuaState(const LuaState&) = delete;
        LuaState(LuaState&& other);
        virtual ~LuaState() {};

        void Register(const t_char* funcName, lua_CFunction func);

        bool          CheckStack(int requiredStackSlots); // TODO: test behaviour
        int           GetTop() const;
        void            SetTop(int stackIndex);

        Lua::Type       Type(int stackIndex) const;
        const t_char*   Typename(int stackIndex) const;

        template <Lua::Type _T>
        bool          IsType(int stackIndex) const
        {
            return Type(stackIndex) == _T;
        }

        template <class _T>
        _T* ToUserData(int stackIndex) const
        {
            return static_cast<_T*>(lua_touserdata(GetState(), stackIndex));
        }

        template <typename _T>
        _T            To(int stackIndex) const
        {
            return ToUserData<std::remove_pointer_t<_T>>(stackIndex);
        }

        int UpValueIndex(int upValueIndex) const
        {
            return lua_upvalueindex(upValueIndex);
        }

        template <typename _T>
        _T            Opt(int stackIndex, const _T& defaultValue) const;

        template <class _T>
        void PushLightUserData(_T* pointer)
        {
            lua_pushlightuserdata(GetState(), pointer);
        }

        void PushCClosure(lua_CFunction func, int numUpValues)
        {
            lua_pushcclosure(GetState(), func, numUpValues);
        }

        void PushNil()
        {
            lua_pushnil(GetState());
        }

        void            Push() {}
        void            Push(bool value);
        void            Push(int value);
        void            Push(float value);
        const t_char*   Push(const t_char* value);

        template <typename _T>
        void Push(_T* value)
        {
            PushLightUserData(value);
        }

        template<typename _FirstArg, typename... _MoreArgs>
        void            Push(_FirstArg&& firstArg, _MoreArgs&&... moreArgs)
        {
            Push(std::forward<_FirstArg>(firstArg));
            Push(std::forward<_MoreArgs>(moreArgs)...);
        }

        void            Pop(int numElements); // TODO: what happens with negative values?
        void            Remove(int stackIndex); // TODO: what happens OOB?

        void            PushValue(int sourceIndex); // TODO: what happens when indexing OOB?
        void            Insert(int targetIndex); // TODO: test what happens if inserting outside stack
        void            Replace(int targetIndex); // TODO: test what happens if inserting outside stack

        void            CreateTable(int narr = 0, int nrec = 0); // TODO: narr and nrec means? and can they be negative...?

        Lua::Type       GetTable(int stackIndex);
        void            SetTable(int stackIndex);

        Lua::Type       GetField(int stackIndex, const t_char* key);
        void            SetField(int stackIndex, const t_char* key);

        bool          GetMetatable(int stackIndex);
        void            SetMetatable(int stackIndex);

        Lua::Type       GetGlobal(const t_char* name);
        void            SetGlobal(const t_char* name);

        void            PushGlobal() // todo: implement
        {
            // push value
            // set global
        }

        Lua::Type       RawGet(int stackIndex);
        void            RawSet(int stackIndex);
        Lua::Type       RawGetI(int stackIndex, int tableIndex);
        void            RawSetI(int stackIndex, int tableIndex);
        size_t        RawLen(int stackIndex) const;

        bool          Next(int stackIndex);

        Lua::ErrorCode LoadFile(const t_char* fileName);
        Lua::ErrorCode DoFile(const t_char* fileName);
        Lua::ErrorCode PCall(int argCount = 0, int retArgCount = LUA_MULTRET);

    protected:
        lua_State* GetState() const { return m_state.get(); }

    private:
        Lua::StateUniquePtr m_state;

    };
}

#include "state.inl"

#endif // CLOUD_LUA_CPP_STATE_HEADER

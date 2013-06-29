#include <iostream>
#include <lua.hpp>

using std::cout;
using std::endl;

void dumpstack(lua_State* L )
{
    int i;
    int top = lua_gettop(L);

    cout << "--- Stack Dump Start" << endl;

    for (i = 1; i <= top; i++)
    {
        cout << i << " " << luaL_typename(L,i) << " - ";
        switch (lua_type(L, i))
        {
            case LUA_TNUMBER:
                cout << lua_tonumber(L,i) << endl;
            break;
            case LUA_TSTRING:
                cout << lua_tostring(L,i) << endl;
                break;
            case LUA_TBOOLEAN:
                cout << (lua_toboolean(L, i) ? "true" : "false") << endl;
                break;
            case LUA_TNIL:
                cout << "nil\n";
                break;
            default:
                cout << std::hex << "0x" << lua_topointer(L,i) << endl;
                break;
        }
    }

    cout << "--- Stack Dump End" << endl;

    return;
}

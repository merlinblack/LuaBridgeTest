#include <cstdlib>
#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>

using std::cout;
using std::endl;
using luabridge::LuaRef;
using luabridge::getGlobal;

void dumpstack( lua_State* );

void test( lua_State* L )
{
    dumpstack( L );

    const char *source =
        "function test( a, b, c )\n"
        "    print( a, b, c )\n"
        "    return a, b, c\n"
        "end\n"
        "";

    cout << source << endl;

    if( luaL_dostring( L, source ) )
    {
        cout << lua_tostring( L, -1 ) << endl;
        lua_pop( L, 1 );
        return;
    }

    dumpstack( L );

    LuaRef test = getGlobal( L, "test" );

    dumpstack( L );

    test( 1, 2, 3 );

    dumpstack( L );

    return;
}

int main()
{
    lua_State* L = luaL_newstate();

    luaL_openlibs( L );

    test( L );

    lua_close( L );

    return EXIT_SUCCESS;
}

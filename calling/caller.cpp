#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>

using std::cout;
using std::endl;
using luabridge::LuaRef;
using luabridge::getGlobal;

void test( lua_State* L )
{
    cout << "Stack top: " << lua_gettop( L ) << endl;

    const char *source =
        "function test( a, b, c )\n"
        "    print( a, b, c )\n"
        "    return a, b, c\n"
        "end\n"
        "";

    cout << source << endl;

    if( luaL_dostring( L, source ) )
    {
        cout << lua_tostring( L, -1 );
        lua_pop( L, 1 );
        return;
    }

    cout << "Stack top: " << lua_gettop( L ) << endl;

    LuaRef test = getGlobal( L, "test" );

    cout << "Stack top: " << lua_gettop( L ) << endl;

    test( 1, 2, 3 );

    cout << "Stack top: " << lua_gettop( L ) << endl;

    return;
}

int main()
{
    lua_State* L = luaL_newstate();

    luaL_openlibs( L );

    test( L );

    return EXIT_SUCCESS;
}

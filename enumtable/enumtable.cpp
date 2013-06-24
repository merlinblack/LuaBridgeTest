#include <cstdlib>
#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>

using std::cout;
using std::endl;
using namespace luabridge;

void test( lua_State* L )
{
    getGlobalNamespace( L )
        .beginNamespace( "MyClasses" )
        .endNamespace();

    LuaRef enums = newTable( L );

    enums["A"] = 1;
    enums["B"] = 2;
    enums["C"] = 3;

    LuaRef MyClasses = getGlobal( L, "MyClasses" );

    MyClasses["Enum"] = enums;

    const char *source =
        "function test()\n"
        "  print( MyClasses.Enum.A )\n"
        "  print( MyClasses.Enum.B )\n"
        "  print( MyClasses.Enum.C )\n"
        "end\n"
        "";

    cout << source << endl;

    if( luaL_dostring( L, source ) )
    {
        cout << lua_tostring( L, -1 ) << endl;
        lua_pop( L, 1 );
        return;
    }

    LuaRef test = getGlobal( L, "test" );

    test();

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

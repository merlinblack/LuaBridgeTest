#include <cstdlib>
#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>

using std::cout;
using std::endl;
using namespace luabridge;

void something( lua_State* L )
{
    cout << "*splode" << endl;
    //luaL_error( L, "Some error." );
    throw "Some Error";
}

int at_panic( lua_State* L )
{
    const char* errormsg = lua_tostring( L, -1 );

    cout << "PANIC: " << errormsg << endl;

    return 0; // So what happens if we return from a panic?
}

void test( lua_State* L )
{
    getGlobalNamespace( L )
        .addFunction( "somethingawful", &something );

    const char *source =
        "function test()\n"
        "   local mt = { __gc = function() print 'About to...' somethingawful() end }\n"
        "   a = {}\n"
        "   setmetatable( a, mt )\n"
        "   return\n"
        "end\n"
        "print( _VERSION )\n"
        "";

    cout << source << endl;

    if( luaL_dostring( L, source ) )
    {
        cout << lua_tostring( L, -1 ) << endl;
        lua_pop( L, 1 );
        return;
    }

    LuaRef testLua = getGlobal( L, "test" );

    testLua();

    return;
}

int main()
{
    lua_State* L = luaL_newstate();

    luaL_openlibs( L );

    lua_atpanic( L, at_panic );

    test( L );

    // Should trigger __gc meta method.
    // What happends when an error is thrown?
    try
    {
        lua_close( L );
    }
    catch( const char* e )
    {
        cout << "Caught error: " << e << endl;
    }

    return EXIT_SUCCESS;
}

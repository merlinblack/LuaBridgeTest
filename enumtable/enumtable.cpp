#include <cstdlib>
#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>

using std::cout;
using std::endl;
using namespace luabridge;

/* ---------------------------------------------------
 * Read only proxy table stuff
 */
int readonlyTableError( lua_State* L )
{
    return luaL_error( L, "Attempt to modify a read only table." );
}

int readonlyTableNext( lua_State* L )
{
    lua_getmetatable( L, 1 ); // Get read only table's metatable
    lua_getfield( L, 3, "__index" ); // Get actual table
    lua_pushvalue( L, 2 );
    if( lua_next( L, 4 ) )
    {
        // Is there a better way?
        //
        lua_remove( L, 3 ); // Meta table
        lua_remove( L, 3 ); // Real table
        lua_remove( L, 2 ); // Input index
        lua_remove( L, 1 ); // Input table
        return 2;
    }
    else
    {
        return 0;
    }
}

int getReadonlyTableNext( lua_State* L )
{
    lua_pushcfunction( L, readonlyTableNext );
    lua_insert( L, 1 );
    lua_pushnil( L );
    return 3;
}

LuaRef makeReadonlyProxy( LuaRef table )
{
    if( table.isTable() == false )
        throw std::invalid_argument( "makeTableReadOnly not given a table as argument" );

    lua_State* L = table.state();

    LuaRef metatable = newTable( L );
    metatable["__index"] = table;
    metatable["__newindex"] = readonlyTableError;
    metatable["__metatable"] = false;
    metatable["__pairs"] = getReadonlyTableNext;

    LuaRef proxy = newTable( L );
    proxy.push( L );
    metatable.push( L );
    lua_setmetatable( L, -2 );
    lua_pop( L, 1 );

    return proxy;
}

/* ---------------------------------------------------
 * End of Read only proxy table stuff
 */

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

    // You can't do this in Lua - the table is protected via metatable.
    MyClasses["Enum"] = makeReadonlyProxy( enums );

    const char *source =
        "function test()\n"
        "  print( 'test()' )\n"
        "  print( MyClasses.Enum.A )\n"
        "  print( MyClasses.Enum.B )\n"
        "  print( MyClasses.Enum.C )\n"
        "end\n"
        "function test2()\n"
        "  print( 'test2()' )\n"
        "  MyClasses.Enum.A = 5\n"
        "end\n"
        "";

    cout << source << endl;

    if( luaL_dostring( L, source ) )
    {
        cout << lua_tostring( L, -1 ) << endl;
        lua_pop( L, 1 );
        return;
    }

    try
    {
        LuaRef test = getGlobal( L, "test" );

        test();

        LuaRef test2 = getGlobal( L, "test2" );

        test2(); // Should throw exception
    }
    catch( std::exception &e )
    {
        cout << e.what() << endl;
    }

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

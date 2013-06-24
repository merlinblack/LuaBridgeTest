#include <cstdlib>
#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>

using std::cout;
using std::endl;
using namespace luabridge;

int nopeReadOnly( lua_State* L )
{
    return luaL_error( L,  "Attempt to modify readonly table.  You scoundrel." );
}
void test( lua_State* L )
{
    getGlobalNamespace( L )
        .beginNamespace( "MyClasses" )
        .endNamespace();

    LuaRef enums = newTable( L );

    enums["A"] = 1;
    enums["B"] = 2;
    enums["C"] = 3;

    // Simple write protecting - has some flaws
    LuaRef metatable = newTable( L );
    metatable["__index"] = enums;
    metatable["__newindex"] = nopeReadOnly;
    metatable["__metatable"] = false;
    LuaRef readonlyEnums = newTable(L);
    LuaRef setmetatable = getGlobal( L,  "setmetatable" );
    setmetatable( readonlyEnums, metatable );
    //
    // This works to make the table read only unless a script
    // uses table.insert, or rawset.  Also nested tables are not
    // protected. There are ways of doing this - see lua-users.org
    // A __pairs metamethod may be needed too.

    LuaRef MyClasses = getGlobal( L, "MyClasses" );

    // You can't do this in Lua - the table is protected via metatable.
    MyClasses["Enum"] = readonlyEnums;

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

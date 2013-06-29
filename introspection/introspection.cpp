#include <cstdlib>
#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>

using std::cout;
using std::endl;
using namespace luabridge;

void dumpstack(lua_State* L );

struct SomeClass
{
    int _i;
    int _x;

    int getX() const { return _x; }
    void setX( int x ) { _x = x; }

    void SomeMember()
    {
        cout << "SomeMember C++" << endl;
    }

    void SomeOtherMember( int i )
    {
        _i = i;
    }

    virtual ~SomeClass() {}
};

int getClassInfo( lua_State* L )
{
    dumpstack( L );

    if( lua_istable( L, 1 ) )
    {
        // Class Type
        cout << "Class Type" << endl;

        // TODO - perhaps get instance metatable onto the stack and fall
        // through to code below.

        return 0;
    }

    if( lua_isuserdata( L, 1 ) )
    {
        // Class instance
        cout << "Class Instance" << endl;

        LuaRef ret = newTable( L );

        lua_getmetatable( L, 1 );
        // List keys where they don't start with underscore.
        lua_pushnil( L );
        while( lua_next( L, -2 ) )
        {
            lua_pushvalue( L, -2 ); // Push copy of key.
            const char *key = lua_tostring( L, -1 );

            if( key )
            {
                if( key[0] != '_' )
                {
                    ret.append( key );
                }
            }

            lua_pop( L, 2 ); // Key copy and value
        }
        // List properties
        lua_pushstring( L, "__propget" );
        lua_rawget( L, -2 );
        if( lua_istable( L, -1 ) )
        {
            lua_pushnil( L );
            while( lua_next( L, -2 ) )
            {
                lua_pushvalue( L, -2 ); // Push copy of key.
                const char *key = lua_tostring( L, -1 );

                if( key )
                {
                    ret.append( key );
                }

                lua_pop( L, 2 ); // Key copy and value
            }
        }

        ret.push( L );

        dumpstack(L);

        return 1;
    }

    return luaL_error( L, "Argument given was not a class type or class instance" );
}

void test( lua_State* L )
{
    getGlobalNamespace( L )
        .addCFunction( "getClassInfo", &getClassInfo )
        .beginNamespace( "MyClasses" )
            .beginClass<SomeClass>("SomeClass")
                .addConstructor<void (*)(void)>()
                .addFunction( "SomeMember", &SomeClass::SomeMember )
                .addFunction( "SomeOtherMember", &SomeClass::SomeOtherMember )
                .addData( "i", &SomeClass::_i )
                .addProperty( "x", &SomeClass::getX, &SomeClass::setX )
            .endClass()
        .endNamespace();

    const char *source =
        "dofile '../interactive.lua'\n"
        "function test()\n"
        "  s = MyClasses.SomeClass()\n"
        "  for k, v in pairs( getClassInfo( s ) ) do\n"
        "    print( k, v )\n"
        "  end\n"
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

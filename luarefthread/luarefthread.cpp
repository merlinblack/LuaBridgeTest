#include <cstdlib>
#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>

using std::cout;
using std::endl;
using namespace luabridge;

void dumpstack(lua_State* L ); // From dumpstack.cpp

struct SomeClass
{
    SomeClass( lua_State* L ) : overide(L) {}

    LuaRef overide;
    virtual void SomeMember()
    {
        if( overide.isFunction() )
            overide();
        else
        {
            cout << "Overide is not a function" << endl;
        }
    }
};

void test( lua_State* L )
{
    getGlobalNamespace( L )
        .beginClass<SomeClass>("SomeClass")
        .addConstructor<void (*)(lua_State*)>()
        .addFunction( "SomeMember", &SomeClass::SomeMember )
        .addData( "SomeMemberOveride", &SomeClass::overide )
        .endClass();

    const char *source =
        "function test()\n"
        "    c:SomeMember()\n"
        "    c.SomeMemberOveride = MyHandler\n"
        "    c:SomeMember()\n"
        "    --This is pretty cool too!\n"
        "    c:SomeMemberOveride()\n"
        "    --Revert to C++ version\n"
        "    c.SomeMemberOveride = nil\n"
        "    c:SomeMember()\n"
        "    return\n"
        "end\n"
        "function MyHandler()\n"
        "    print 'SomeMember Overidden by Lua'\n"
        "end\n"
        "";

    // Use a thread to make Lua object
    const char *threadSource =
        "c = SomeClass()\n";

    lua_State* thread1 = lua_newthread( L );
    lua_State* thread2 = lua_newthread( L );

    cout << "Main    pointer:" << std::hex << L << endl;
    cout << "Thread1 pointer:" << std::hex << thread1 << endl;
    cout << "Thread2 pointer:" << std::hex << thread2 << endl;

    cout << "Thread:\n" << threadSource << endl;

    if( luaL_dostring( thread1, threadSource ) )
    {
        cout << lua_tostring( thread1, -1 ) << endl;
        lua_pop( thread1, 1 );
        return;
    }

    // Release first thread...
    lua_remove( L, 1 );
    lua_gc( L, LUA_GCCOLLECT, 0 );
    cout << "First thread is now gone." << endl;

    cout << "Another thread:\n" << source << endl;

    if( luaL_dostring( thread2, source ) )
    {
        cout << lua_tostring( thread2, -1 ) << endl;
        lua_pop( thread2, 1 );
        return;
    }

    LuaRef test = getGlobal( thread2, "test" );

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

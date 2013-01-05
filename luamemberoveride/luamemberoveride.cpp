#include <cstdlib>
#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>

using std::cout;
using std::endl;
using namespace luabridge;

struct SomeClass
{
    virtual void SomeMember()
    {
        cout << "SomeMember C++" << endl;
    }
    virtual ~SomeClass() {}
};

struct SomeClassWrapper : public SomeClass
{
    SomeClassWrapper( lua_State* L ) : overide(L) {}

    LuaRef overide;
    virtual void SomeMember()
    {
        if( overide.isFunction() )
            overide();
        else
            SomeClass::SomeMember();
    }
};

void test( lua_State* L )
{
    getGlobalNamespace( L )
        .beginNamespace( "MyClasses" )
            .beginClass<SomeClassWrapper>("SomeClass")
                .addConstructor<void (*)(lua_State*)>()
                .addFunction( "SomeMember", &SomeClassWrapper::SomeMember )
                .addData( "SomeMemberOveride", &SomeClassWrapper::overide )
            .endClass()
        .endNamespace();

    const char *source =
        "function test()\n"
        "    c = MyClasses.SomeClass()\n"
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

#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>

using std::cout;
using std::endl;
using namespace luabridge;

struct Base
{
    int a;
    virtual ~Base() {}
};

struct Derived : public Base
{
    int b;
    virtual ~Derived() {}
};

template<class D, class B>
D* downcast( B* ptr )
{
    return dynamic_cast<D*>( ptr );
}

void test( lua_State* L )
{
    getGlobalNamespace( L )
        .beginNamespace( "MyClasses" )
            .beginClass<Base>("Base")
                .addData( "a", &Base::a )
            .endClass()
            .deriveClass<Derived,Base>("Derived")
                .addData( "b", &Derived::b )
                .addStaticFunction( "downcast", &downcast<Derived, Base> )
            .endClass()
        .endNamespace();

    const char *source =
        "function test( c )\n"
        "    print( 'Contents of \"a\" member', c.a )\n"
        "    print( 'Contents of \"b\" member', c.b )\n"
        "    return\n"
        "end\n"
        "function test2( c )\n"
        "    d = MyClasses.Derived.downcast( c )\n"
        "    if( d ~= nil ) then\n"
        "        test( d )\n"
        "    else\n"
        "        print 'Could not downcast'\n"
        "    end\n"
        "end\n"
        "";

    cout << source << endl;

    if( luaL_dostring( L, source ) )
    {
        cout << lua_tostring( L, -1 );
        lua_pop( L, 1 );
        return;
    }

    Derived o;

    o.a = 42;
    o.b = 84;

    Base* b = &o;
    Derived* d = &o;

    LuaRef test = getGlobal( L, "test" );
    LuaRef test2 = getGlobal( L, "test2" );

    cout << "Calling test with base pointer." << endl;
    test( b );
    cout << endl;

    cout << "Calling test with derived pointer." << endl;
    test( d );
    cout << endl;

    cout << "Calling test2 with base pointer." << endl;
    test2( b );
    cout << endl;

    cout << "Calling test2 with derived pointer." << endl;
    test2( d );
    cout << endl;

    Base allyourbase;

    cout << "Calling test2 with base pointer to base object." << endl;
    test2( &allyourbase );
    cout << endl;

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

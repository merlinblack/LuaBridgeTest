#include <cstdlib>
#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>
#include <RefCountedObject.h>

using std::cout;
using std::endl;
using namespace luabridge;

struct Widget : public RefCountedObject
{
    Widget() : something( 42 )
    {
        cout << "Created widget." << endl;
    }
    ~Widget()
    {
        cout << "Destroyed widget." << endl;
    }
    int something;

    typedef RefCountedObjectPtr<Widget> Ptr;
};

Widget::Ptr makewidget()
{
    cout << "Creating widget with factory function." << endl;
    return new Widget;
}

int destroyWidget( lua_State* L )
{
    cout << "Destroying widget with factory function." << endl;

    // Some preprocessing before letting the object get reaped.
    {
        LuaRef p1 = LuaRef::fromStack( L, 1 );

        /*
           if( p1.is<Widget::Ptr>() == false )
           return luaL_error( L, "trying to destroy a non widget!!!" );
           */

        Widget::Ptr ptr = LuaRef_cast<Widget::Ptr>( p1 );

        // Do something with object before we get rid of it
        ptr->something = 0xdead;
    }

    // Normal Luabridge delete
    Userdata* const ud = Userdata::getExact <Widget> (L, 1);
    ud->~Userdata ();

    return 0;
}

void test( lua_State* L )
{
    getGlobalNamespace( L )
        .addFunction( "makewidget", &makewidget )
        .beginClass<Widget>("Widget")
        .addData("something", &Widget::something )
        .endClass();

    // Override __gc
    lua_getglobal( L, "Widget" );
    lua_getmetatable( L, -1 );
    lua_pushstring( L, "__class" );
    lua_gettable( L, -2 );
    lua_pushstring( L, "__gc" );
    lua_pushcfunction( L, destroyWidget );
    lua_settable( L, -3 );
    lua_pop( L, 3 );

    const char *source =
        "print( _VERSION )\n"
        "\n"
        "a = {}\n"
        "mt = {\n"
        "    __gc = function( self )\n"
        "        print \"Finializer\"\n"
        "        if self.widget then\n"
        "            print( self.widget ) -- these are accessing a deleted object!!!\n"
        "            print( self.widget.something ) -- not good!\n"
        "        end\n"
        "    end\n"
        "}\n"
        "setmetatable( a, mt )\n"
        "a.widget = makewidget()\n"
        "\n"
        "--------------------------------------------------\n"
        "\n"
        "print( a.widget )\n"
        "print( a.widget.something )\n"
        "\n"
        "print \"Setting 'a' to nil.\"\n"
        "a = nil\n"
        "\n"
        "print \"Collect garbage.\"\n"
        "collectgarbage()\n"
        "\n"
        "print 'The end.'\n";

    cout << source << endl;

    if( luaL_dostring( L, source ) )
    {
        cout << lua_tostring( L, -1 ) << endl;
        lua_pop( L, 1 );
        return;
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

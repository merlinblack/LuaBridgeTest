#include <cstdlib>
#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>
#include <RefCountedObject.h>
#include <map>

using std::cout;
using std::endl;
using namespace luabridge;

struct Widget : public RefCountedObject
{
    Widget( int n ) : something( n )
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

typedef std::map<int, Widget::Ptr> WidgetList;
typedef std::map<int, Widget::Ptr>::iterator WidgetListIter;

WidgetList gWidgets;

LuaRef getWidgetList( lua_State* L )
{
    LuaRef table = newTable(L);

    for( WidgetListIter i = gWidgets.begin(); i != gWidgets.end(); i++ )
    {
        table[i->first] = i->second;
    }

    return table;
}

void test( lua_State* L )
{
    getGlobalNamespace( L )
        .beginClass<Widget>("Widget")
        .addData("something", &Widget::something )
        .endClass()
        .addFunction( "getWidgets", &getWidgetList );

    gWidgets.insert( std::pair<int, Widget::Ptr>( 1, new Widget(10) ) );
    gWidgets.insert( std::pair<int, Widget::Ptr>( 2, new Widget(20) ) );
    gWidgets.insert( std::pair<int, Widget::Ptr>( 3, new Widget(30) ) );

    const char *source =
        "print( _VERSION )\n"
        "function dump(t)\n"
        "  print( 'Dump:' )\n"
        "  for k,v in pairs(t) do\n"
        "    print( k, v.something )\n"
        "  end\n"
        "end\n";

    cout << source << endl;

    if( luaL_dostring( L, source ) )
    {
        cout << lua_tostring( L, -1 ) << endl;
        lua_pop( L, 1 );
        return;
    }

    for( int i=0; i <= 500; i++ )
    {
        luaL_dostring( L, "dump( getWidgets() )" );
        lua_gc( L, LUA_GCCOLLECT, 0 );
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

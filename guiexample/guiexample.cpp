#include <iostream>
#include <string.h>
#include <lua.hpp>
#include <LuaBridge.h>

using std::cout;
using std::endl;
using namespace luabridge;

// Some GUI class that represents a 2D rectangular region.
// This could be a 'window' or a 'button', etc.
//
// ( Like Betajean's Gorilla's rectangle. )
//

class Rectangle
{
    int _x, _y, _width, _height;

    public:
    Rectangle( int x, int y, int w, int h ) : _x(x), _y(y), _width(w), _height(h) {}

    bool isInside( int tx, int ty )
    {
        if( tx < _x )
            return false;
        if( ty < _y )
            return false;
        if( tx > _x + _width )
            return false;
        if( ty > _y + _height )
            return false;

        return true;
    }
    virtual ~Rectangle() {}
};

void test( lua_State* L )
{
    getGlobalNamespace( L )
        .beginNamespace( "GUI" )
            .beginClass<Rectangle>("Rectangle")
                .addConstructor<void (*)(float,float,float,float)>()
                .addFunction( "isInside", &Rectangle::isInside )
            .endClass()
        .endNamespace();

    if( luaL_dofile( L, "test.lua" ) )
    {
        cout << lua_tostring( L, -1 );
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

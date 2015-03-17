#### Common make file settings for all sub projects.
#
LIBS:=-L$(HOME)/prog/lua-5.3.0/src -llua -ldl
CXXFLAGS:=-g -Wall -Wno-deprecated -I$(HOME)/prog/LuaBridge/Source/LuaBridge -I$(HOME)/prog/lua-5.3.0/src
OBJDIR=obj
DEPDIR=deps

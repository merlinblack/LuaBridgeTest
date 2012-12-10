# Nigels wizzzy "simple" makefile
# ###################################################################
#
LIBS=-L$(HOME)/prog/local/lib -llua -ldl
CXXFLAGS=-g -Wall -Wno-deprecated -I$(HOME)/prog/local/include
EXECUTABLE=guiexample
SOURCES=guiexample.cpp
OBJDIR=obj
DEPDIR=deps

# ###################################################################

.SILENT: $(DEPS) $(DEPDIR) $(OBJDIR) clean

OBJECTS=$(addprefix $(OBJDIR)/,$(SOURCES:.cpp=.o))
DEPS=$(addprefix $(DEPDIR)/,$(SOURCES:.cpp=.d))

.PHONY: all
all:	$(OBJDIR) $(SOURCES) $(DEPS) gitversioning.h $(EXECUTABLE)

$(EXECUTABLE):	$(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

# Squeeky
# ########
.PHONY: clean
clean:
	echo 'Cleaning'
	rm -f $(OBJECTS) $(EXECUTABLE) gitversioning.h
	rm -rf $(DEPDIR)
	rm -rf $(OBJDIR)

# Object files
# #############
#
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Dependency files
# #################
#
# Make sure gitversioning.h is made before trying to make
# any dependency file

$(DEPDIR)/%.d: %.cpp gitversioning.h | $(DEPDIR)
	@echo "Caching dependency info for: $<"
	@$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

$(DEPDIR):
	mkdir -p $(DEPDIR)

ifeq (0,$(words $(findstring clean, $(MAKECMDGOALS))))
	# Don't make these, only to clean them out again.
-include $(DEPS)
endif

# gitversioning.h is remade when either git tags or heads change.
gitversioning.h: ../.git/refs/tags ../.git/refs/heads ../.git/HEAD
	@../gitversioning.sh

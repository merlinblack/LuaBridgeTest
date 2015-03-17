.PHONY: all
all:
	$(MAKE) -C luamemberoveride
	$(MAKE) -C autodowncast
	$(MAKE) -C calling
	$(MAKE) -C guiexample
	$(MAKE) -C enumtable
	$(MAKE) -C introspection
	$(MAKE) -C stdlist
	$(MAKE) -C splode
	$(MAKE) -C refcount
	$(MAKE) -C luarefthread

.PHONY: clean
clean:
	$(MAKE) -C luamemberoveride clean
	$(MAKE) -C autodowncast clean
	$(MAKE) -C calling clean
	$(MAKE) -C guiexample clean
	$(MAKE) -C enumtable clean
	$(MAKE) -C introspection clean
	$(MAKE) -C stdlist clean
	$(MAKE) -C splode clean
	$(MAKE) -C refcount clean
	$(MAKE) -C luarefthread clean

.PHONY: all
all:
	$(MAKE) -C luamemberoveride
	$(MAKE) -C autodowncast
	$(MAKE) -C calling
	$(MAKE) -C guiexample

.PHONY: clean
clean:
	$(MAKE) -C luamemberoveride clean
	$(MAKE) -C autodowncast clean
	$(MAKE) -C calling clean
	$(MAKE) -C guiexample clean

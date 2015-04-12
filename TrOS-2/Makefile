#TrOS-2 Makefile

MODULES := bootloader/stage1
SUBCLEAN = $(addsuffix .clean,$(MODULES))

.PHONY: $(MODULES)

rebuild: clean all

all: $(MODULES)

$(MODULES):
	$(MAKE) -C $@

clean: $(SUBCLEAN)

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

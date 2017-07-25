.PHONY: all clean

DOSCOM_CONFIG ?= config.txt
DOSCOM_INSTALL_DIR ?= ../../asp/obj/doscom/

all:
	@gcc -I. -Wall -DCONFIG_FILE=\"$(DOSCOM_CONFIG)\" configurator.c -o configurator.exe

install:
	@mv rtos-doscom_config.h $(DOSCOM_INSTALL_DIR)/trust/doscom_config.h
	@mv rtos-doscom.cfg $(DOSCOM_INSTALL_DIR)/trust/doscom.cfg
	@mv gpos-doscom_config.h $(DOSCOM_INSTALL_DIR)/non-trust/doscom_config.h

clean:
	@find \( -name '*.[oa]' -or -name '*~' -or -name '*.a' -or -name '*.exe' \) -delete
	@rm -f configurator-out.c

distclean: clean
	@rm -f rtos-doscom_config.h gpos-doscom_config.h rtos-doscom.cfg

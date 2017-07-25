#
# TOPPERS/SafeG Dual-OS monitor
#     Toyohashi Open Platform for Embedded Real-Time Systems/
#     Safety Gate Dual-OS monitor
#
# Copyright (C) 2009-2013 by Embedded and Real-Time Systems Laboratory
#    Graduate School of Information Science, Nagoya University, JAPAN
#
# 上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
# ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
# 変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
# (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
#     権表示，この利用条件および下記の無保証規定が，そのままの形でソー
#     スコード中に含まれていること．
# (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
#     用できる形で再配布する場合には，再配布に伴うドキュメント（利用
#     者マニュアルなど）に，上記の著作権表示，この利用条件および下記
#     の無保証規定を掲載すること．
# (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
#     用できない形で再配布する場合には，次のいずれかの条件を満たすこ
#     と．
#   (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
#       作権表示，この利用条件および下記の無保証規定を掲載すること．
#   (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
#       報告すること．
# (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
#     害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
#     また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
#     由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
#     免責すること．
#
# 本ソフトウェアは，無保証で提供されているものである．上記著作権者お
# よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
# に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
# アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
# の責任を負わない．
#
# [Makefile]
#
# This file contains the Makefile used to build the monitor. A
# target-dependent Makefile (Makefile.target) is also included for adding
# additional flags, files and set up target-dependent options.
#
# The monitor is built using the following command:
#
# $ make {list of 'CONFIGURABLE_VARIABLE=xxx'}
#
# Where CONFIGURABLE_VARIABLE is any of the configurable variables existing
# either in this 'Makefile' or in 'Makefile.target'
#
# For a detailed explanation of each configurable variable, please consult
# SafeG's documentation (e.g., 'safeg_build_process.txt').
#
.PHONY: all clean

# Top Dir 
SAFEGDIR = ..

# Include global setting
include $(SAFEGDIR)/Makefile.config

# Application library directory
APPLIBDIR = ./libsafeg

# Build-related configurable variables.
ENABLE_DEBUG      ?= y
ENABLE_SPEED      ?= n
ENABLE_VERBOSE    ?= 1
TARGET_NAME       ?= $(MONITOR_TARGET)
TOOLCHAIN_PREFIX  ?= arm-none-eabi

# Monitor-related configurable variables
MONITOR_OMIT_BSS_INIT      ?= 0
MONITOR_OMIT_DATA_INIT     ?= 1
MONITOR_OMIT_T_CHECKS      ?= 0
MONITOR_TRAP_NT_IRQS       ?= 0
MONITOR_TRAP_T_FIQS        ?= 1
MONITOR_T_FIQS_TO_IRQS     ?= 1
MONITOR_TRAP_EXT_ABORTS    ?= 0
MONITOR_ENABLE_SYSCALLS    ?= 1
MONITOR_ENABLE_NOTIFIERS   ?= 0
MONITOR_MAX_NOTIFIERS      ?= 32
MONITOR_MAX_DYN_SYSCALLS   ?= 32
MONITOR_T_SAVE_SVC_ONLY    ?= 0
MONITOR_NT_HIGH_VECTOR     ?= 0
MONITOR_T_SAVE_VFPV3_NEON  ?= 1
MONITOR_NT_SAVE_VFPV3_NEON ?= 1

include target/$(TARGET_NAME)/Makefile.target

CC      = $(TOOLCHAIN_PREFIX)-gcc
OBJCOPY = $(TOOLCHAIN_PREFIX)-objcopy
NM      = $(TOOLCHAIN_PREFIX)-nm

DEBUG-y = -g
DEBUG-n =
SPEED-y = -O2 -fomit-frame-pointer
SPEED-n = -O0

ifeq ("$(ENABLE_VERBOSE)","1")
Q =
else
Q = @
endif

LDFLAGS += -mcpu=$(TARGET_CPU) -msoft-float -static -nostdlib \
		-Wl,-Ttext,$(TARGET_MONITOR_START_ADDRESS)

ifdef TARGET_MONITOR_DATA_START_ADDRESS
	LDFLAGS += -Wl,-Tdata,$(TARGET_MONITOR_DATA_START_ADDRESS)
endif

CFLAGS  += -std=c99 -mcpu=$(TARGET_CPU) -Wall -Wextra -Werror      \
		$(DEBUG-$(ENABLE_DEBUG)) $(SPEED-$(ENABLE_SPEED))          \
		-I . -I syscalls/ -I common/ -I notifier/                  \
		-I target/$(TARGET_NAME)/                                  \
		-DTARGET_T_OS_START_ADDRESS=$(TARGET_T_OS_START_ADDRESS)   \
		-DTARGET_NT_OS_START_ADDRESS=$(TARGET_NT_OS_START_ADDRESS) \
		-DTARGET_ARCH_VERSION=$(TARGET_ARCH_VERSION)               \
		-DTARGET_CPU=$(TARGET_CPU)                                 \
		-DTARGET_NUM_CORES=$(TARGET_NUM_CORES)                     \
		-DTARGET_PRIMARY_CORE_ID=$(TARGET_PRIMARY_CORE_ID)         \
		-DMONITOR_OMIT_BSS_INIT=$(MONITOR_OMIT_BSS_INIT)           \
		-DMONITOR_OMIT_DATA_INIT=$(MONITOR_OMIT_DATA_INIT)         \
		-DMONITOR_TRAP_NT_IRQS=$(MONITOR_TRAP_NT_IRQS)             \
		-DMONITOR_TRAP_T_FIQS=$(MONITOR_TRAP_T_FIQS)               \
		-DMONITOR_T_FIQS_TO_IRQS=$(MONITOR_T_FIQS_TO_IRQS)         \
		-DMONITOR_TRAP_EXT_ABORTS=$(MONITOR_TRAP_EXT_ABORTS)       \
		-DMONITOR_ENABLE_SYSCALLS=$(MONITOR_ENABLE_SYSCALLS)       \
		-DMONITOR_ENABLE_NOTIFIERS=$(MONITOR_ENABLE_NOTIFIERS)     \
		-DMONITOR_MAX_NOTIFIERS=$(MONITOR_MAX_NOTIFIERS)           \
		-DMONITOR_MAX_DYN_SYSCALLS=$(MONITOR_MAX_DYN_SYSCALLS)     \
		-DMONITOR_T_SAVE_SVC_ONLY=$(MONITOR_T_SAVE_SVC_ONLY)       \
		-DMONITOR_T_SAVE_VFPV3_NEON=$(MONITOR_T_SAVE_VFPV3_NEON)   \
		-DMONITOR_NT_SAVE_VFPV3_NEON=$(MONITOR_NT_SAVE_VFPV3_NEON)


ifeq ($(MONITOR_T_SAVE_VFPV3_NEON),1)
CFLAGS += -mhard-float -mfloat-abi=softfp -mfpu=neon
else
ifeq ($(MONITOR_NT_SAVE_VFPV3_NEON),1)
CFLAGS += -mhard-float -mfloat-abi=softfp -mfpu=neon
else
CFLAGS += -msoft-float
endif
endif

SOURCES = start.S startup.c monitor.S monitor_asm.inc monitor.h    \
		common/arm.h common/arm_asm.inc common/types.h          \
		common/utils.h common/error.h common/debug.h            \
		syscalls/syscalls.c syscalls/syscalls.h                 \
		notifier/notifier.c notifier/notifier.h                 \
		notifier/notifier_asm.inc                               \
		$(addprefix target/$(TARGET_NAME)/, $(TARGET_SOURCES))

OBJECTS = $(filter %.o, $(SOURCES:.c=.o) $(SOURCES:.S=.o))

DEPEND  = Makefile linker.ld \
		$(addprefix target/$(TARGET_NAME)/, $(TARGET_DEPEND))

all: monitor.bin $(APPLIBDIR)/libsafeg.a

monitor.bin: $(OBJECTS) $(DEPEND)
	$(Q)$(CC) $(LDFLAGS) -T linker.ld $(OBJECTS) -lgcc -o monitor.elf
	$(Q)$(OBJCOPY) -O binary -S monitor.elf monitor.bin
	$(Q)$(NM) -g monitor.elf > monitor.sym
	@echo "Monitor was built succesfully"

$(APPLIBDIR)/libsafeg.a:
	make -C $(APPLIBDIR)

notifier/%.o: notifier/%.c
	$(Q)$(CC) -c $(CFLAGS) $< -o $@

syscalls/%.o: syscalls/%.c
	$(Q)$(CC) -c $(CFLAGS) $< -o $@

target/$(TARGET_NAME)/%.o: target/$(TARGET_NAME)/%.c
	$(Q)$(CC) -c $(CFLAGS) $< -o $@

%.o: %.S
	$(Q)$(CC) -c $(CFLAGS) $<

%.o: %.c
	$(Q)$(CC) -c $(CFLAGS) $<

-include Makefile.depend

Makefile.depend:
	$(Q)$(CC) $(CFLAGS) -MM $(filter %.c, $(SOURCES)) \
		$(filter %.S, $(SOURCES)) > Makefile.depend

clean:
	@find \( -name '*.elf' -or -name '*.bin' -or -name '*.sym'        \
		 -or -name '*~*' -or -name '*.o' -or -name '*.depend' \)  \
		 -print -delete
	make clean -C $(APPLIBDIR)

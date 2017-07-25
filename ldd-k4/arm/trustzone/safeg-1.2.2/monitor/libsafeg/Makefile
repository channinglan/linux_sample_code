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
# @(#) $Id: Makefile 606 2013-11-19 03:08:19Z ertl-honda $
.PHONY: all clean

# Top Dir 
SAFEGDIR = ../../

# Include global setting
include $(SAFEGDIR)/Makefile.config

# Include monitor target Makefile
include $(SAFEGDIR)/monitor/target/$(MONITOR_TARGET)/Makefile.target

TOOLCHAIN_PREFIX ?= arm-none-eabi
ENABLE_DEBUG     ?= y
ENABLE_SPEED     ?= n
ENABLE_VERBOSE   ?= 1

CC      = $(TOOLCHAIN_PREFIX)-gcc
OBJCOPY = $(TOOLCHAIN_PREFIX)-objcopy
NM      = $(TOOLCHAIN_PREFIX)-nm

DEBUG-y = -g
DEBUG-n =
SPEED-y = -O2
SPEED-n = -O0

ifeq ("$(ENABLE_VERBOSE)","1")
Q =
else
Q = @
endif

CFLAGS  = -std=c99 -mcpu=$(TARGET_CPU) -msoft-float -Wall -Wextra -Werror -I . \
		-nostdlib -nostdinc -fomit-frame-pointer                \
		$(DEBUG-$(ENABLE_DEBUG)) $(SPEED-$(ENABLE_SPEED))

SOURCES = syscalls_api.h syscalls_api.c

OBJECTS = $(filter %.o, $(SOURCES:.c=.o))

DEPEND  = Makefile

all: libsafeg.a

libsafeg.a: $(OBJECTS) $(DEPEND)
	$(Q)rm -f libsafeg.a
	$(Q)ar -r libsafeg.a *.o
	@echo "Library was built succesfully"

%.o: %.c
	$(Q)$(CC) -c $(CFLAGS) $<

-include Makefile.depend

Makefile.depend:
	$(Q)$(CC) $(CFLAGS) -MM $(filter %.c, $(SOURCES)) > Makefile.depend

clean:
	@find \( -name '*.a' -or -name '*~*' -or -name '*.o' -or \
		 -name '*.depend' \)  \
		 -print -delete

# Copyright (c) 2015, Wind River Systems, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

#
# Main Makefile that is invoked when the C9 build buttons/menus are
# used.  This is invoked in the contect of a BASH shell with all of
# the normal Wind River Linux sysroot variables set.
#

-include app.mk

#
# If WIND_PROJECT_ROOT is in the environment, set the VXE_DIR to be the
# bin subdirectory.  If not, maintain the legacy behaviour and put it in
# the obj subdirectory.
# 
# Additioanlly, if WIND_PROJECT_ROOT is set, use it for SOURCEDIR else
# maintain the legacy behaviour and make it `pwd`.
#
ifeq ($(WIND_PROJECT_ROOT),)
VXE_DIR = $(shell pwd)/obj
SOURCEDIR=$(WIND_PROJECT_ROOT)
else
VXE_DIR = $(WIND_PROJECT_ROOT)/bin
SOURCEDIR=$(shell pwd)
endif

#
# If WIND_PROJECT_NAME is in the environment, set the EXE name to be the
# same as WIND_PROJECT_NAME.  If not, maintain the legacy behaviour and set
# it to be the name in resources.mk or the of the top level project 
# directory
#
ifeq ($(WIND_PROJECT_NAME),)
-include resources.mk
ifeq ($(EXE),)
EXE= $(shell basename `pwd`)
endif
else
EXE=$(WIND_PROJECT_NAME)
endif

#
# Include VxWorks-7 make rules

include $(WIND_USR_MK)/defs.default.mk
include $(WIND_KRNL_MK)/defs.toolfamily.mk
include $(WIND_BUILD_TOOL)/$(TOOL_VERSION)/usr/defs.$(CPU).mk

#
# Find all source files in the project tree that are not in a .dot directory
# or a .dot file.
#

CFILES = $(shell find $(SOURCEDIR) -name '*.c' | grep  -v  '/\.[a-zA-Z]*')
CXXFILES = $(shell find $(SOURCEDIR) -name '*.cc' -o -name '*.cpp' | grep  -v  '/\.[a-zA-Z]*')
ASFILES = $(shell find $(SOURCEDIR) -name '*.s' | grep  -v  '/\.[a-zA-Z]*')
OBJS = $(addsuffix .o,$(notdir $(basename $(CFILES)) $(basename $(ASFILES)) $(basename $(CXXFILES))))
OBJ_DIR=obj

EXE_STRIP=$(EXE).debug

$(VXE_DIR)/$(EXE_STRIP): $(VXE_DIR)/$(EXE)
	cp $(VXE_DIR)/$(EXE) $(VXE_DIR)/$(EXE_STRIP)

default: $(VXE_DIR)/$(EXE)

define _catsub
	@echo $1 >> $2

endef

dirname = $(patsubst %/,%,$(dir $1))

$(foreach f, $(CFILES) $(ASFILES) $(CXXFILES), \
    $(eval SRC_DIRS += $(patsubst %/,%,$(dir $f))))

vpath %.c $(SRC_DIRS)
vpath %.s $(SRC_DIRS)
vpath %.cc $(SRC_DIRS)
vpath %.cpp $(SRC_DIRS)

ifneq ($(subst diab,,$(TOOL)),$(TOOL))
EXTRA_DEFINE	+=-O0 -g $(CC_DEBUG_STACK) -Xdebug-dwarf3
else
ifneq ($(subst 4_3_3,,$(TOOL_VERSION)),$(TOOL_VERSION))
EXTRA_DEFINE	+=-O0 -g
else
EXTRA_DEFINE	+=-O0 -g -gdwarf-3
endif
endif

#
# Utility rules
#
.PHONY: clean prjclean
clean:  prjclean

prjclean:
	rm -rf $(OBJDIR)
	rm -f bin/*

.PHONY: vars
showvars:
	echo WIND_PROJECT_ROOT: $(WIND_PROJECT_ROOT)
	echo WIND_PROJECT_NAME: $(WIND_PROJECT_NAME)
	@echo VXE_DIR: $(VXE_DIR)
	@echo EXE: $(EXE)
	@echo Default target: $(EXEDIR)/$(EXE)
	@echo OBJS: $(OBJS)
	@echo SRC_DIRS $(SRC_DIRS)


#
# Include rest of VxWorks RTP Make rules
#
include $(WIND_USR_MK)/rules.rtp.mk


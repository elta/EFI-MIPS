#/*++
#   
#  Copyright (c) 2004 - 2007, Intel Corporation                                                         
#  All rights reserved. This program and the accompanying materials                          
#  are licensed and made available under the terms and conditions of the BSD License         
#  which accompanies this distribution.  The full text of the license may be found at        
#  http://opensource.org/licenses/bsd-license.php                                            
#                                                                                            
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             
#  
#  Module Name:  makefile
#   
#  Abstract:
#   
#    This file is used to build the EFI utility.
#   
#--*/

#
# Do this if you want to compile from this directory
#
ifndef TOOLCHAIN
TOOLCHAIN = TOOLCHAIN_GCC
endif

include $(BUILD_DIR)/PlatformTools.env

#
# Common information
#

INC +=

#
# Target specific information
#

TARGET_NAME=GenCRC32Section

TARGET_SOURCE_DIR = $(EDK_TOOLS_SOURCE)/$(TARGET_NAME)

TARGET_EXE = $(EDK_TOOLS_OUTPUT)/$(TARGET_NAME)

TARGET_EXE_SOURCE = $(TARGET_SOURCE_DIR)/GenCRC32Section.c
TARGET_EXE_INCLUDE = $(EDK_SOURCE)/Foundation/Include/TianoCommon.h \
                     $(EDK_SOURCE)/Foundation/Framework/Include/EfiFirmwareFileSystem.h \
                     $(EDK_SOURCE)/Foundation/Framework/Include/EfiFirmwareVolumeHeader.h \
                     $(EDK_TOOLS_COMMON)/ParseInf.h
TARGET_EXE_LIBS = $(EDK_TOOLS_OUTPUT)/Common.a

LIBS += -lgcc --as-needed \
			 -lgcc_s --no-as-needed \
			 -lc

#
# Build targets
#

all: $(TARGET_EXE)

#
# Build EXE
#

$(EDK_TOOLS_OUTPUT)/$(TARGET_NAME).o: $(TARGET_EXE_SOURCE) $(TARGET_EXE_INCLUDE)
	$(CC) $(C_FLAGS) $(INC) $(TARGET_EXE_SOURCE) -o $(EDK_TOOLS_OUTPUT)/$(TARGET_NAME).o

#
# Add Binary Build description for this tool.
#

ifeq ($(EFI_BINARY_TOOLS),YES)
ifneq ($(wildcard $(EFI_PLATFORM_BIN)/Tools/$(TARGET_NAME)),)
$(TARGET_EXE): $(EFI_PLATFORM_BIN)/Tools/$(TARGET_NAME)
	$(CP) $(EFI_PLATFORM_BIN)/Tools/$(TARGET_NAME) $(TARGET_EXE)
else
$(TARGET_EXE): $(EDK_TOOLS_OUTPUT)/$(TARGET_NAME).o $(TARGET_EXE_LIBS) $(TARGET_DLL)
	$(LINK)    \
		$(GCC_LINK_LIBPATHS) \
		$(DYNAMIC_LINKER) \
		$(LINK_FLAGS_EXE) \
		-o $(TARGET_EXE)  \
		$(CRT_BEGIN_OBJ)  \
		$(EDK_TOOLS_OUTPUT)/$(TARGET_NAME).o  \
		$(TARGET_LIB)   \
		$(TARGET_EXE_LIBS) \
		$(LIBS)  \
		$(CRT_END_OBJ)
	@if [ -d $(EFI_PLATFORM_BIN)/Tools ] ; then \
	$(ECHO) exist $(EFI_PLATFORM_BIN)/Tools > $(NULL) ; \
	else \
	$(MKDIR) $(EFI_PLATFORM_BIN)/Tools ; \
	fi
	@if [ -e $(TARGET_EXE) ] ; then \
  $(CP) $(TARGET_EXE) $(EFI_PLATFORM_BIN)/Tools/$(TARGET_NAME) ; \
	fi
endif
else
$(TARGET_EXE): $(EDK_TOOLS_OUTPUT)/$(TARGET_NAME).o $(TARGET_EXE_LIBS) $(TARGET_DLL)
	$(LINK)    \
		$(GCC_LINK_LIBPATHS) \
		$(DYNAMIC_LINKER) \
		$(LINK_FLAGS_EXE) \
		-o $(TARGET_EXE)  \
		$(CRT_BEGIN_OBJ)  \
		$(EDK_TOOLS_OUTPUT)/$(TARGET_NAME).o  \
		$(TARGET_LIB)   \
		$(TARGET_EXE_LIBS) \
		$(LIBS)  \
		$(CRT_END_OBJ)
	@if [ -d $(EFI_PLATFORM_BIN)/Tools ] ; then \
	$(ECHO) exist $(EFI_PLATFORM_BIN)/Tools > $(NULL) ; \
	else \
	$(MKDIR) $(EFI_PLATFORM_BIN)/Tools ; \
	fi
	@if [ -e $(TARGET_EXE) ] ; then \
  $(CP) $(TARGET_EXE) $(EFI_PLATFORM_BIN)/Tools/$(TARGET_NAME) ; \
	fi
endif

clean:
	$(RM) $(EDK_TOOLS_OUTPUT)/$(TARGET_NAME).*

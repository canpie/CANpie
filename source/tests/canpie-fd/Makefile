#=============================================================================#
# Makefile for project: CANpie                                                #
# CANpie driver       : template                                              #
#=============================================================================#



#-----------------------------------------------------------------------------#
# Target name                                                                 #
#                                                                             #
#-----------------------------------------------------------------------------#
TARGET     = test_canpie
TARGET_HW  = Simulation


#-----------------------------------------------------------------------------#
# Debug code generation                                                       #
#                                                                             #
#-----------------------------------------------------------------------------#
DEBUG      = 0


#-----------------------------------------------------------------------------#
# Path setup (source and object directory)                                    #
#                                                                             #
#-----------------------------------------------------------------------------#

#---------------------------------------------------------------
# PRJ_DIR: absolute or relative path to project root directory
#
PRJ_DIR	= .

#---------------------------------------------------------------
# DEV_DIR: path to device directory
#
DEV_DIR 	= $(PRJ_DIR)/../../device/template

#---------------------------------------------------------------
# CAN_DIR: path to canpie-fd directory
#
CAN_DIR  = $(PRJ_DIR)/../../canpie-fd

#---------------------------------------------------------------
# TEST_DIR: path to test directory
#
TEST_DIR	= $(PRJ_DIR)

#----------------------------------------------------------
# Object directory
#
OBJ_DIR	= $(PRJ_DIR)


#-----------------------------------------------------------------------------#
# Compiler settings                                                           #
#                                                                             #
#-----------------------------------------------------------------------------#
ifeq ($(OS),Windows_NT)
	CC ="D:/devtools/cygwin/cygwin64/bin/gcc"
	SPLINT=  "d:/devtools/splint/splint-3.1.2/bin/splint.exe"
else

endif


#---------------------------------------------------------------
# Include directory for header files
#
INC_DIR   = -I $(DEV_DIR)
INC_DIR  += -I $(CAN_DIR)
INC_DIR  += -I $(TEST_DIR)

#---------------------------------------------------------------
# Set VPATH to the same value like include paths
# but without '-I'
VPATH =$(INC_DIR:-I= )
ALL_CFILES = $(wildcard $(CAN_DIR)/*.c $(DEV_DIR)/*.c)

#---------------------------------------------------------------
# Warning level
#
WARN  = -Wall
WARN += -Wextra 
WARN += -Wmissing-include-dirs -Winit-self 
WARN += -Wswitch-enum -Wundef -Wshadow 
WARN += -Wbad-function-cast -Wcast-qual 
WARN += -Wpacked -Wcast-align -Wswitch-default
WARN += -std=c99 
WARN += -pedantic


#---------------------------------------------------------------
# TARGET CPU & FPU
# CPU : CPU architecture for -mcpu, possible values
#       cortex-m3
#       cortex-m4
# FPU : defines FPU support for -mfloat-abi, possible values
#       soft
#       softfp
#       hard
#
CPU = 
FPU = 


#---------------------------------------------------------------
# Check for debug option flag
#
ifeq ($(DEBUG),1)
	GDB_FLAG = -gdwarf-2 -g
	OPTIMIZE	= -O0
else
	GDB_FLAG = 
	OPTIMIZE	= -O1 
endif

#---------------------------------------------------------------
# Specific user/application symbol definition
# 
MC_FLAG  = 
ifeq ($(OS),Windows_NT)
MC_FLAG += -D_WIN32=1 
endif


#-----------------------------------------------------------------------------#
# GCC compiler and linker settings                                            #
#                                                                             #
#-----------------------------------------------------------------------------#

#--------------------------------------------------------------------
# Compiler FLAGS
#
CFLAGS	 = $(CPU) $(FPU)  $(MC_FLAG)
CFLAGS	+= $(OPTIMIZE) $(WARN) $(INC_DIR)
CFLAGS	+= -c -funsigned-char  -nostdlib 


#--------------------------------------------------------------------
# Linker FLAGS
#
LFLAGS	= $(CPU) $(FPU)
LFLAGS  += $(GDB_FLAG)


#-----------------------------------------------------------------------------#
# MISRA-C checker settings																		#
# The TI checker generates false positives for 10.1, 10.5 and 17.6   			#
#-----------------------------------------------------------------------------#

MISRA_DIR   = /Applications/TexasInstruments/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS
MCC   		= $(MISRA_DIR)/bin/armcl
MFLAGS      = --check_misra="all,-2.2,-5.7,-10.1,-10.5,-17.6" 
MFLAGS     += --misra_advisory=warning --misra_required=warning
MFLAGS     += $(INC_DIR)
MFLAGS 	  += -I $(MISRA_DIR)/include

#-----------------------------------------------------------------------------#
# List of object files that need to be compiled                               #
#                                                                             #
#-----------------------------------------------------------------------------#


#--------------------------------------------------------------------
# CANpie source files 
#
#--------------------------------------------------------------------
CAN_SRC  = cp_msg.c	


#--------------------------------------------------------------------
# Device / target CPU source files 
# 
#--------------------------------------------------------------------
DEV_SRC	=	

#device_canfd.c


#--------------------------------------------------------------------
# Unit test source
# compiled if TEST_OBJS==1
#--------------------------------------------------------------------

FUNC_SRC =	test_cp_main_f.c			\
				test_cp_msg_ccf.c			\
				test_cp_msg_fdf.c			\
				unity_fixture.c			\
				unity.c

MACRO_SRC =	test_cp_main_m.c			\
				test_cp_msg_ccm.c			\
				test_cp_msg_fdm.c			\
				unity_fixture.c			\
				unity.c


#--------------------------------------------------------------------
# generate list of all required object files
#
#--------------------------------------------------------------------
FUNC_OBJS  = $(patsubst %.c,$(OBJ_DIR)/%.o, $(DEV_SRC))
FUNC_OBJS += $(patsubst %.c,$(OBJ_DIR)/%.o, $(FUNC_SRC))
FUNC_OBJS += $(patsubst %.c,$(OBJ_DIR)/%.o, $(CAN_SRC))

MACRO_OBJS  = $(patsubst %.c,$(OBJ_DIR)/%.o, $(DEV_SRC))
MACRO_OBJS += $(patsubst %.c,$(OBJ_DIR)/%.o, $(MACRO_SRC))
MACRO_OBJS += $(patsubst %.c,$(OBJ_DIR)/%.o, $(CAN_SRC))

#-----------------------------------------------------------------------------#
# Rules                                                                       #
#                                                                             #
#-----------------------------------------------------------------------------#
all: msg_func msg_macro
	@echo - Done
	
msg_func: $(FUNC_OBJS) 
	@echo Build target $(TARGET)_func
	@echo - Linking : Target is $(TARGET)_func ...
	@$(CC) $(LFLAGS) -o $(OBJ_DIR)/$(TARGET)_func $(FUNC_OBJS)	

msg_macro: $(MACRO_OBJS) 
	@echo Build target $(TARGET)_macro
	@echo - Linking : Target is $(TARGET)_macro ...
	@$(CC) $(LFLAGS) -o $(OBJ_DIR)/$(TARGET)_macro $(MACRO_OBJS)	
		
check:
	@splint -f splint.rc $(ALL_CFILES)

misra:
	$(MCC) $(MFLAGS) $(ALL_CFILES)
show:
	@echo TARGET_OBJS:
	@echo $(TARGET_OBJS)
	@echo INC_DIR:
	@echo $(INC_DIR) 
	@echo VPATH:
	@echo $(VPATH)
	@echo ALL_CFILES:
	@echo $(ALL_CFILES)

run:
	@$(OBJ_DIR)/$(TARGET)
	

msg_compiling:
	@echo Build target: $(TARGET) 

docs:
	cd $(DOC_DIR)
	doxygen
	
clean:
	@rm -f $(OBJ_DIR)/*.o
	@rm -f $(OBJ_DIR)/*.d 
	@rm -f ./$(TARGET)_func 
	@rm -f ./$(TARGET)_macro

#-----------------------------------------------------------------------------#
# Dependencies                                                                #
#                                                                             #
#-----------------------------------------------------------------------------#

#--- standard C files -------------------------------------
$(OBJ_DIR)/%.o : %.c
	@echo - Compiling : $(<F)
	@$(CC) $(CFLAGS) $< -o $@ -MMD
 

#--------------------------------------------------------------------
# include header files dependencies
#
#--------------------------------------------------------------------
-include $(patsubst %.o,%.d, $(TARGET_OBJS))

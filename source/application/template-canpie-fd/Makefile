#=============================================================================#
# Makefile for project: CANpie                                                #
# CANpie driver       : template                                              #
#=============================================================================#



#-----------------------------------------------------------------------------#
# Target name								                                          #
#																										#
#-----------------------------------------------------------------------------#
TARGET     = canpie
TARGET_HW  = Simulation


#-----------------------------------------------------------------------------#
# Debug code generation                                                       #
#																										#
#-----------------------------------------------------------------------------#
DEBUG      = 0


#-----------------------------------------------------------------------------#
# Path setup (source and object directory)												#
#																										#
#-----------------------------------------------------------------------------#

#---------------------------------------------------------------
# PRJ_DIR: absolute or relative path to project root directory
#
PRJ_DIR	= ../..


#---------------------------------------------------------------
# DEV_DIR: path to device directory
#
DEV_DIR 	= $(PRJ_DIR)/device/template

#---------------------------------------------------------------
# TEST_DIR: path to test directory
#
TEST_DIR	= $(PRJ_DIR)/tests

#---------------------------------------------------------------
# DOC_DIR: path to documentation directory
#
DOC_DIR 	= $(PRJ_DIR)/docs/doxygen

#----------------------------------------------------------
# Object directory
#
OBJ_DIR	= $(PRJ_DIR)/../objs


#-----------------------------------------------------------------------------#
# Compiler settings (GNU-ARM compiler)														#
#																										#
#-----------------------------------------------------------------------------#


#---------------------------------------------------------------
# Include directory for header files
#
INC_DIR   = -I $(DEV_DIR)
INC_DIR  += -I $(PRJ_DIR)/include
INC_DIR  += -I $(TEST_DIR)

#---------------------------------------------------------------
# Set VPATH to the same value like include paths
# but without '-I'
VPATH =$(INC_DIR:-I= )
ALL_CFILES = $(wildcard $(PRJ_DIR)/*/*.c $(PRJ_DIR)/*/*.h)

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
#WARN += -pedantic


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
	OPTIMIZE	= -O1 -fomit-frame-pointer -mlittle-endian
endif

#---------------------------------------------------------------
# Specific user/application symbol definition
# 
MC_FLAG  = 
#MC_FLAG += -DUNITY_OUTPUT_CHAR=UnityOutputChar



#-----------------------------------------------------------------------------#
# GCC compiler and linker settings															#
#																										#
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
# List of object files that need to be compiled											#
#																										#
#-----------------------------------------------------------------------------#


#--------------------------------------------------------------------
# Application source files 
#
#--------------------------------------------------------------------
APP_SRC =	



#--------------------------------------------------------------------
# Device / target CPU source files 
# 
#--------------------------------------------------------------------
DEV_SRC	=	device_can.c			



#--------------------------------------------------------------------
# Unit test source
# compiled if TEST_OBJS==1
#--------------------------------------------------------------------
TEST_SRC =	cp_test_core.c				\
				cp_test_main.c				\
				cp_test_msg.c				\
				unity_fixture.c			\
				unity.c
		
				
#--------------------------------------------------------------------
# generate list of all required object files
#
#--------------------------------------------------------------------
TARGET_OBJS  = $(patsubst %.c,$(OBJ_DIR)/%.o, $(DEV_SRC))
TARGET_OBJS	+= $(patsubst %.c,$(OBJ_DIR)/%.o, $(TEST_SRC))



#-----------------------------------------------------------------------------#
# Rules																								#
#																										#
#-----------------------------------------------------------------------------#
all: msg_compiling $(TARGET_OBJS)
	@echo - Linking : Target is $(TARGET).elf for $(TARGET_HW) ...
	@$(CC) $(LFLAGS) -o $(OBJ_DIR)/$(TARGET) $(TARGET_OBJS)
	@cp $(OBJ_DIR)/$(TARGET) .
	
show:
	@echo TARGET_OBJS:
	@echo $(TARGET_OBJS)
	@echo INC_DIR:
	@echo $(INC_DIR) 
	@echo VPATH:
	@echo $(VPATH)
	@echo ALL_CFILES:
	@echo $(ALL_CFILES)


msg_compiling:
	echo Build target: $(TARGET) 

docs:
	cd $(DOC_DIR)
	doxygen
	
clean:
	@rm -f $(OBJ_DIR)/*.o
	@rm -f $(OBJ_DIR)/*.d 
	@rm -f $(OBJ_DIR)/*.hex
	@rm -f $(OBJ_DIR)/*.map
	@rm -f $(OBJ_DIR)/*.bin
	@rm -f $(OBJ_DIR)/*.elf
	@rm -f ./$(TARGET).hex 

#-----------------------------------------------------------------------------#
# Dependencies																						#
#																										#
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

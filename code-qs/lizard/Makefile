#=============================================================================#
# Makefile for project: MCL                                                   #
# Application         : lizard                                                #
# https://github.com/terryyin/lizard                                          #
#=============================================================================#



#-----------------------------------------------------------------------------#
# Target name                                                                 #
#                                                                             #
#-----------------------------------------------------------------------------#
TARGET     = mcl_lizard
TARGET_HW  = simulation


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
PRJ_DIR		= ./../../..

#---------------------------------------------------------------
# SRC_DIR: source
#
SRC_DIR		= $(PRJ_DIR)/source

#---------------------------------------------------------------
# DEV_DIR: path to DEV directory
#
DEV_DIR  	= $(SRC_DIR)/application/template-device

#---------------------------------------------------------------
# EG_DIR: example directory
#
EG_DIR 		= $(SRC_DIR)/examples

#----------------------------------------------------------
# Object directory
#
OBJ_DIR		= $(PRJ_DIR)/objs


#-----------------------------------------------------------------------------#
# Compiler settings                                                           #
#                                                                             #
#-----------------------------------------------------------------------------#
ifeq ($(OS),Windows_NT)
	LIZARD	= "./../../../../lizard-master/lizard.py"
else
	LIZARD = "./../../../../lizard/lizard.py"
endif


#-----------------------------------------------------------------------------#
# Lizard Rules                                                                #
# https://github.com/terryyin/lizard                                          #
#-----------------------------------------------------------------------------#
#---------------------------------------------------------------------
# Maximal number lines of code without comments
#
LENGTH = 500

#---------------------------------------------------------------------
# Maximal cyclomatic complexity number
#
CCN = 15

#---------------------------------------------------------------------
# Maximal number of parameters of a function
#
ARGUMENTS = 6 

#---------------------------------------------------------------------
# Target language
#
LANGUAGE = cpp

#-----------------------------------------------------------------------------#
# Rules                                                                       #
#                                                                             #
#-----------------------------------------------------------------------------#
lizard_all:
	@python $(LIZARD) -l $(LANGUAGE) -m -C $(CCN) -L $(LENGTH) -a $(ARGUMENTS) -x "*.h" $(DEV_DIR)

lizard_warn:
	@echo 'NLOC (lines of code without comments): max. $(LENGTH)' 
	@echo 'CCN (cyclomatic complexity number): max. $(CCN)'
	@echo 'token count of functions: does not care'
	@echo 'PARAM (parameter Number): max. $(ARGUMENTS)' 
	@python $(LIZARD) -w -l $(LANGUAGE) -m -C $(CCN) -L $(LENGTH) -a $(ARGUMENTS) -x "*.h" $(DEV_DIR)
	

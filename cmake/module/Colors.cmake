#----------------------------------------------------------------------------------------------------------------------#
# This CMake include defines color definitions for console output                                                      #
#                                                                                                                      #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#


#-------------------------------------------------------------------------------------------------------
# some color definitions
#
# Colorized Output Setup
if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(Bold        "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Blue        "${Esc}[34m")
  set(Cyan        "${Esc}[36m")
  set(Magenta     "${Esc}[35m")
  set(Yellow      "${Esc}[33m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldWhite   "${Esc}[1;37m")
endif()


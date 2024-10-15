#----------------------------------------------------------------------------------------------------------------------#
# This CMake include defines compiler flags for the project                                                            #
#                                                                                                                      #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

if(APPLE)
   set(COMMON_COMPILER_FLAGS  "-Wall -Wextra -Wpedantic")

   #----------------------------------------------------------------------------------------------------
   # The Clang compiler has more warning options than GCC
   #
   if ("${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")

      set(COMMON_COMPILER_FLAGS  "${COMMON_COMPILER_FLAGS} -Weverything")
     
      #--------------------------------------------------------------------------------------------
      # exclude some warnings
      #--------------------------------------------------------------------------------------------
      set(COMMON_COMPILER_FLAGS  "${COMMON_COMPILER_FLAGS} -Wno-disabled-macro-expansion -Wno-deprecated-declarations -Wno-switch-enum -Wno-padded")
      set(COMMON_COMPILER_FLAGS  "${COMMON_COMPILER_FLAGS} -Wno-documentation")

   endif()

   if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")

      set(COMMON_COMPILER_FLAGS  "${COMMON_COMPILER_FLAGS} -Weverything")
  
      #--------------------------------------------------------------------------------------------
      # exclude some warnings
      #--------------------------------------------------------------------------------------------
      set(COMMON_COMPILER_FLAGS  "${COMMON_COMPILER_FLAGS} -Wno-deprecated-declarations -Wno-switch-enum -Wno-padded")
      set(COMMON_COMPILER_FLAGS  "${COMMON_COMPILER_FLAGS} -Wno-documentation")

endif()

   set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${COMMON_COMPILER_FLAGS}")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILER_FLAGS} -Wno-weak-vtables -Wno-old-style-cast")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors -Wno-exit-time-destructors")

elseif (("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC"))
   #---------------------------------------------------------------------------------------------------------- 
   # Here some hints when using MSVC 
   #
   # Compiler Warning (level 4) C4710: 
   #  <https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4710?view=msvc-170>
   #  Such warning can be disabled by passing /wd4710 to the compiler
   #
   # Compiler Warning (level 1) C4711:
   #  <https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4711?view=msvc-170>
   #  Such warning can be disabled by passing /wd4711 to the compiler
   #
   # Compiler Warning (level 4) C4820:
   #  <https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4820?view=msvc-170>
   #  Such warning can be disabled by passing /wd4820 to the compiler
   #
   # Compiler Warning (level 4) C4100
   # <https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4100?view=msvc-170>
   #  Such warning can be disabled by passing /wd4100 to the compiler
   # 
   # Compiler Warning C5045
   # <https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/c5045?view=msvc-170>
   # Such warning can be disabled by passing /wd5045 to the compiler
   #
   # Compiler Warning (level 4) C4061
   # <https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4061?view=msvc-170>
   # Such warning can be disabled by passing /wd4061 to the compiler
   #
   set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall /wd4710 /wd4711 /wd4820 /wd4100 /wd5045 /wd4061")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall /wd4191 /wd4710 /wd4711 /wd4820 /wd4100 /wd5045 /wd4061 /wd4464 /wd4866")
endif()

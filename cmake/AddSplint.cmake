INCLUDE(FindPackageHandleStandardArgs)

FIND_PROGRAM(SPLINT_EXECUTABLE splint )

FIND_PACKAGE_HANDLE_STANDARD_ARGS(splint DEFAULT_MSG SPLINT_EXECUTABLE)

# only visible in advanced view
MARK_AS_ADVANCED(SPLINT_EXECUTABLE)

FUNCTION(add_splint TARGET)
    if(${SPLINT_FOUND})
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(SPLINT_FLAGS -D__GNUC__ -D__APPLE__ -f ${CP_PATH_TEST}/splint/splint.rc)
        elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
            set(SPLINT_FLAGS -D__GNUC__ -D__linux__ -f ${CP_PATH_TEST}/splint/splint.rc)
        elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
            set(SPLINT_FLAGS -D_MSC_VER -f ${CP_PATH_TEST}/splint/splint.rc)
        else()
	        message(WARNING "I don't know how to configure splint for this platform.")
        endif()

        get_directory_property(include_dirs INCLUDE_DIRECTORIES)

        foreach(incdir ${include_dirs})
            list(APPEND include_flags -I${incdir})
        endforeach()

        add_custom_target(
            ${TARGET}_splint
            COMMAND ${SPLINT_EXECUTABLE} ${SPLINT_FLAGS} ${include_flags} ${ARGN}
            DEPENDS ${ARGN}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            )
        #add_dependencies(${TARGET} ${TARGET}_splint)
    endif()
ENDFUNCTION()
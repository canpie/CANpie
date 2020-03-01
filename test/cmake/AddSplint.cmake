INCLUDE(FindPackageHandleStandardArgs)

FIND_PROGRAM(SPLINT_EXECUTABLE splint )

FIND_PACKAGE_HANDLE_STANDARD_ARGS(splint DEFAULT_MSG SPLINT_EXECUTABLE)

# only visible in advanced view
MARK_AS_ADVANCED(SPLINT_EXECUTABLE)

FUNCTION(add_splint TARGET)
    if(${SPLINT_FOUND})

        set(SPLINT_FLAGS -f ${CMAKE_MODULE_PATH}/splint.rc)

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
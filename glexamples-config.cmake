
######
# This is the find-script for your project/library used by CMake if this project/libary is to be
# included in another project/library.
######

#
# Renaming GLEXAMPLES... -> <PROJECT_NAME>...
#
# GLEXAMPLES_DIR -> <PROJECT_NAME>_DIR
# GLEXAMPLES_FOUND -> <PROJECT_NAME>_FOUND
# GLEXAMPLES_LIBRARIES -> <PROJECT_NAME>_LIBRARIES
# GLEXAMPLES_INCLUDES -> <PROJECT_NAME>_INCLUDES

# GLEXAMPLES_FIBLIB_LIBRARY -> <PROJECT_NAME>_<...>_LIBRARY
# GLEXAMPLES_FIBLIB_LIBRARY_RELEASE -> <PROJECT_NAME>_<...>_LIBRARY_RELEASE
# GLEXAMPLES_FIBLIB_LIBRARY_DEBUG -> <PROJECT_NAME>_<...>_LIBRARY_DEBUG
# GLEXAMPLES_FIBLIB_INCLUDE_DIR -> <PROJECT_NAME>_<...>_INCLUDE_DIR

set(GLEXAMPLES_INCLUDES "")
set(GLEXAMPLES_LIBRARIES "")

# Definition of function "find" with two mandatory arguments, "LIB_NAME" and "HEADER".
macro (find LIB_NAME HEADER)

    set(HINT_PATHS ${ARGN})

    if (${LIB_NAME} STREQUAL "glexamples")
        set(LIB_NAME_UPPER GLEXAMPLES)
        set(LIBNAME glexamples)
    else()
        string(TOUPPER GLEXAMPLES_${LIB_NAME} LIB_NAME_UPPER)
        set(LIBNAME ${LIB_NAME})
    endif()

    find_path(
	${LIB_NAME_UPPER}_INCLUDE_DIR
	${HEADER}
        ${ENV_GLEXAMPLES_DIR}/include
        ${ENV_GLEXAMPLES_DIR}/source/${LIB_NAME}/include
        ${GLEXAMPLES_DIR}/include
        ${GLEXAMPLES_DIR}/source/${LIB_NAME}/include
        ${ENV_PROGRAMFILES}/glexamples/include
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
        DOC "The directory where ${HEADER} resides"
    )


    find_library(
	${LIB_NAME_UPPER}_LIBRARY_RELEASE
        NAMES ${LIBNAME}
        PATHS ${HINT_PATHS}
        DOC "The ${LIB_NAME} library"
    )
    find_library(
	${LIB_NAME_UPPER}_LIBRARY_DEBUG
        NAMES ${LIBNAME}d
        PATHS ${HINT_PATHS}
        DOC "The ${LIB_NAME} debug library"
    )


    if(${LIB_NAME_UPPER}_LIBRARY_RELEASE AND ${LIB_NAME_UPPER}_LIBRARY_DEBUG)
        set(${LIB_NAME_UPPER}_LIBRARY "optimized" ${${LIB_NAME_UPPER}_LIBRARY_RELEASE} "debug" ${${LIB_NAME_UPPER}_LIBRARY_DEBUG})
    elseif(${LIB_NAME_UPPER}_LIBRARY_RELEASE)
        set(${LIB_NAME_UPPER}_LIBRARY ${${LIB_NAME_UPPER}_LIBRARY_RELEASE})
    elseif(${LIB_NAME_UPPER}_LIBRARY_DEBUG)
        set(${LIB_NAME_UPPER}_LIBRARY ${${LIB_NAME_UPPER}_LIBRARY_DEBUG})
    endif()

    list(APPEND GLEXAMPLES_INCLUDES ${${LIB_NAME_UPPER}_INCLUDE_DIR})
    list(APPEND GLEXAMPLES_LIBRARIES ${${LIB_NAME_UPPER}_LIBRARY})

    # DEBUG MESSAGES
    # message("${LIB_NAME_UPPER}_INCLUDE_DIR     = ${${LIB_NAME_UPPER}_INCLUDE_DIR}")
    # message("${LIB_NAME_UPPER}_LIBRARY_RELEASE = ${${LIB_NAME_UPPER}_LIBRARY_RELEASE}")
    # message("${LIB_NAME_UPPER}_LIBRARY_DEBUG   = ${${LIB_NAME_UPPER}_LIBRARY_DEBUG}")
    # message("${LIB_NAME_UPPER}_LIBRARY         = ${${LIB_NAME_UPPER}_LIBRARY}")

endmacro(find)


# load standard CMake arguments (c.f. http://stackoverflow.com/questions/7005782/cmake-include-findpackagehandlestandardargs-cmake)
include(FindPackageHandleStandardArgs)

if(CMAKE_CURRENT_LIST_FILE)
    get_filename_component(GLEXAMPLES_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
endif()

file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" ENV_PROGRAMFILES)
file(TO_CMAKE_PATH "$ENV{GLEXAMPLES_DIR}" ENV_GLEXAMPLES_DIR)

set(LIB_PATHS
    ${GLEXAMPLES_DIR}/build
    ${GLEXAMPLES_DIR}/build/Release
    ${GLEXAMPLES_DIR}/build/Debug
    ${GLEXAMPLES_DIR}/build-release
    ${GLEXAMPLES_DIR}/build-debug
    ${GLEXAMPLES_DIR}/lib
    ${ENV_GLEXAMPLES_DIR}/lib
    ${ENV_PROGRAMFILES}/glexamples/lib
    /usr/lib
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    /usr/lib64
    /usr/local/lib64
    /sw/lib64
    /opt/local/lib64
)

# Find libraries
find(fiblib fiblib/fiblib_api.h ${LIB_PATHS})

if(GLEXAMPLES_FIBLIB_LIBRARY)
  # add dependencies
endif()


# DEBUG
# message("GLEXAMPLES_INCLUDES  = ${GLEXAMPLES_INCLUDES}")
# message("GLEXAMPLES_LIBRARIES = ${GLEXAMPLES_LIBRARIES}")

find_package_handle_standard_args(GLEXAMPLES DEFAULT_MSG GLEXAMPLES_LIBRARIES GLEXAMPLES_INCLUDES)
mark_as_advanced(GLEXAMPLES_FOUND)

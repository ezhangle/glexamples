
set(target transparency-painters)


# External libraries

find_package(ASSIMP QUIET)

if (NOT ASSIMP_FOUND)
	message("Example ${target} skipped: ASSIMP not found")
	return()
endif()

message(STATUS "Example ${target}")

# Includes

include_directories(
    BEFORE
    ${ASSIMP_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)


# Libraries

set(libs
    ${GLEXAMPLES_DEPENDENCY_LIBRARIES}
    ${ASSIMP_LIBRARIES}
)


# Compiler definitions



# for compatibility between glm 0.9.4 and 0.9.5
add_definitions("-DGLM_FORCE_RADIANS")


# Sources

set(include_path "${CMAKE_CURRENT_SOURCE_DIR}/")
set(source_path "${CMAKE_CURRENT_SOURCE_DIR}/")

set(sources
    ${source_path}/AssimpLoader.cpp
    ${source_path}/AssimpProcessing.cpp
    ${source_path}/PolygonalDrawable.cpp
    ${source_path}/PolygonalGeometry.cpp
    ${source_path}/plugin.cpp
    ${source_path}/screendoor/ScreenDoor.cpp
    ${source_path}/stochastic/StochasticTransparency.cpp
    ${source_path}/stochastic/StochasticTransparencyOptions.cpp
    ${source_path}/stochastic/MasksTableGenerator.cpp
)

set(api_includes
    ${include_path}/AssimpLoader.h
    ${include_path}/AssimpProcessing.h
    ${include_path}/PolygonalDrawable.h
    ${include_path}/PolygonalGeometry.h
    ${include_path}/screendoor/ScreenDoor.h
    ${include_path}/stochastic/StochasticTransparency.h
    ${include_path}/stochastic/StochasticTransparencyOptions.h
    ${include_path}/stochastic/MasksTableGenerator.h
)

# Group source files
set(header_group "Header Files (API)")
set(source_group "Source Files")
source_group_by_path(${include_path} "\\\\.h$|\\\\.hpp$"
    ${header_group} ${api_includes})
source_group_by_path(${source_path} "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$"
    ${source_group} ${sources})


# Build library

add_library(${target} SHARED ${api_includes} ${sources})

target_link_libraries(${target} ${libs})

target_compile_options(${target} PRIVATE ${DEFAULT_COMPILE_FLAGS})

set_target_properties(${target}
    PROPERTIES
    LINKER_LANGUAGE              CXX
    FOLDER                      "${IDE_FOLDER}"
    COMPILE_DEFINITIONS_DEBUG   "${DEFAULT_COMPILE_DEFS_DEBUG}"
    COMPILE_DEFINITIONS_RELEASE "${DEFAULT_COMPILE_DEFS_RELEASE}"
    LINK_FLAGS_DEBUG            "${DEFAULT_LINKER_FLAGS_DEBUG}"
    LINK_FLAGS_RELEASE          "${DEFAULT_LINKER_FLAGS_RELEASE}"
    DEBUG_POSTFIX               "d${DEBUG_POSTFIX}"
    INCLUDE_PATH                ${include_path})


# Deployment

# Plugin library (release)
install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_PLUGINS}       CONFIGURATIONS Release
    LIBRARY DESTINATION ${INSTALL_PLUGINS}       CONFIGURATIONS Release
    ARCHIVE DESTINATION ${INSTALL_LIB}
)

# Plugin library (debug)
install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_PLUGINS_DEBUG} CONFIGURATIONS Debug
    LIBRARY DESTINATION ${INSTALL_PLUGINS_DEBUG} CONFIGURATIONS Debug
)

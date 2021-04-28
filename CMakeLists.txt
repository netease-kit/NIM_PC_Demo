CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

MESSAGE("############# nim_win_demo ##############")

SET(CMAKE_CXX_STANDARD 11)

IF (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    MESSAGE(STATUS "No CMAKE_BUILD_TYPE selected, defaulting to ${CMAKE_BUILD_TYPE}")
ENDIF ()

SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "default install prefix" FORCE)
SET(BUILD_WITH_CONAN OFF CACHE BOOL "Build project with conan")

IF (CMAKE_BUILD_TYPE MATCHES "Release")
    ADD_DEFINITIONS(-DNDEBUG)
ENDIF ()

IF (CMAKE_CL_64)
ELSE ()
    ADD_DEFINITIONS(-DSUPPORTLOCALPLAYER)
ENDIF ()

SET(CONAN_DISABLE_CHECK_COMPILER ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
SET_PROPERTY(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

SET(GROUP_BASE "base")
SET(GROUP_UI_COMPONENTS "ui_components")
SET(RELEASE_OUTPUT_7Z_FILE "${CMAKE_BINARY_DIR}/release.7z")
STRING(REPLACE "/" "\\\\" RELEASE_OUTPUT_7Z_FILE_WIN ${RELEASE_OUTPUT_7Z_FILE})

PROJECT(nim_win_demo)

# Build project with conan
IF (BUILD_WITH_CONAN)
    # https://github.com/conan-io/cmake-conan
    # Download automatically, you can also just copy the conan.cmake file
    IF (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        MESSAGE(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        FILE(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
    ENDIF()
    INCLUDE(${CMAKE_BINARY_DIR}/conan.cmake)
    conan_cmake_autodetect(settings)
    conan_cmake_install(PATH_OR_REFERENCE .. BUILD missing SETTINGS ${settings})
    INCLUDE(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
ENDIF ()

# Download third parties
IF (CMAKE_BUILD_TYPE MATCHES Debug AND NOT BUILD_WITH_CONAN)
    IF (NOT EXISTS "${CMAKE_BINARY_DIR}/nim_demo_build_libraries_x86_debug.zip")
        MESSAGE(STATUS "Downloading third party libraries from http://yx-web.nos.netease.com/package/1619524144/nim_demo_build_libraries_x86_debug.zip")
        FILE(DOWNLOAD "http://yx-web.nos.netease.com/package/1619524144/nim_demo_build_libraries_x86_debug.zip" "${CMAKE_BINARY_DIR}/nim_demo_build_libraries_x86_debug.zip")
    ENDIF ()
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar -xf "${CMAKE_BINARY_DIR}/nim_demo_build_libraries_x86_debug.zip" WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
ENDIF ()
IF (CMAKE_BUILD_TYPE MATCHES Release AND NOT BUILD_WITH_CONAN)
    IF (NOT EXISTS "${CMAKE_BINARY_DIR}/nim_demo_build_libraries_x86_release.zip")
        MESSAGE(STATUS "Downloading third party libraries from http://yx-web.nos.netease.com/package/1619524201/nim_demo_build_libraries_x86_release.zip")
        FILE(DOWNLOAD "http://yx-web.nos.netease.com/package/1619524201/nim_demo_build_libraries_x86_release.zip" "${CMAKE_BINARY_DIR}/nim_demo_build_libraries_x86_release.zip")
    ENDIF ()
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar -xf "${CMAKE_BINARY_DIR}/nim_demo_build_libraries_x86_release.zip" WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
ENDIF ()

ADD_DEFINITIONS(
    -DUNICODE 
    -D_UNICODE 
    -DPSAPI_VERSION=1
)

# Git information
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/")
INCLUDE(GetGitRevisionDescription)
git_release_version(GIT_TAG)
IF (GIT_TAG STREQUAL "")
    git_latest_tag(GIT_TAG)
ENDIF ()
git_describe(GIT_DESCRIBE)
git_commit_counts(GIT_COMMIT_COUNT)
SET(GIT_TAG "8.4.0")
STRING(REPLACE "." "," GIT_TAG_WITH_COMMA ${GIT_TAG})
MESSAGE(STATUS "Current git tag: ${GIT_TAG}, commit count: ${GIT_COMMIT_COUNT}, describe: ${GIT_DESCRIBE}")

ADD_COMPILE_OPTIONS("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
ADD_COMPILE_OPTIONS("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Gm- /Zi")
SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_LIST_DIR}/lib/
    ${CMAKE_CURRENT_LIST_DIR}/include/
)

FILE(GLOB_RECURSE THIRD_PARTY_LIBS lib/*.lib)

ADD_SUBDIRECTORY(chatroom)
ADD_SUBDIRECTORY(tool_kits/base)
ADD_SUBDIRECTORY(tool_kits/duilib)
ADD_SUBDIRECTORY(tool_kits/shared)
ADD_SUBDIRECTORY(tool_kits/db)
ADD_SUBDIRECTORY(tool_kits/transfer_file)
ADD_SUBDIRECTORY(tool_kits/ui_component/av_kit)
ADD_SUBDIRECTORY(tool_kits/ui_component/g2_kit)
ADD_SUBDIRECTORY(tool_kits/ui_component/capture_image)
ADD_SUBDIRECTORY(tool_kits/ui_component/image_view)
ADD_SUBDIRECTORY(tool_kits/ui_component/nim_service)
ADD_SUBDIRECTORY(tool_kits/ui_component/ui_kit)
ADD_SUBDIRECTORY(tool_kits/cef/cef_module)
ADD_SUBDIRECTORY(tool_kits/cef/cef_render)
ADD_SUBDIRECTORY(tool_kits/cef/cef_wrapper)
ADD_SUBDIRECTORY(app_sdk)
ADD_SUBDIRECTORY(nim_win_demo)
# uninstall
ADD_SUBDIRECTORY(tool_kits/uninstall)

SET_PROPERTY(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT nim_demo)

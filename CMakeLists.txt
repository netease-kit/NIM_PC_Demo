CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

MESSAGE("############# nim_win_demo ##############")

SET(CMAKE_CXX_STANDARD 11)

IF (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    MESSAGE(STATUS "No CMAKE_BUILD_TYPE selected, defaulting to ${CMAKE_BUILD_TYPE}")
ENDIF ()

SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "default install prefix" FORCE)
SET(BUILD_WITH_CONAN OFF CACHE BOOL "Build project with conan")
SET(BUILD_WITH_NERTC_G2 OFF CACHE BOOL "Build project with NeRTC G2")

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

SET(DEBUG_THIRD_PARTY_LIBS "http://yx-web.nos.netease.com/package/1619605746/NIM_Windows_Demo_Build_Binaries_Debug.zip")
SET(RELEASE_THIRD_PARTY_LIBS "http://yx-web.nos.netease.com/package/1619605768/NIM_Windows_Demo_Build_Binaries_Release.zip")
SET(NIM_CROSS_PLATFORM_SDK_URL "http://10.242.100.195/IM_pack/Cross-platform/release/8.10.0/nim-win32-ia32-release-8.10.0.583-build.3.tar.gz")
SET(NERTC_SDK_URL "http://yx-web.nos.netease.com/package/1618217725/NERtc_Windows_SDK_v4.1.1.zip")

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
        MESSAGE(STATUS "Downloading third party libraries from ${DEBUG_THIRD_PARTY_LIBS}")
        FILE(DOWNLOAD ${DEBUG_THIRD_PARTY_LIBS} "${CMAKE_BINARY_DIR}/nim_demo_build_libraries_x86_debug.zip")
    ENDIF ()
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar -xf "${CMAKE_BINARY_DIR}/nim_demo_build_libraries_x86_debug.zip" WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
ENDIF ()
IF (CMAKE_BUILD_TYPE MATCHES Release AND NOT BUILD_WITH_CONAN)
    IF (NOT EXISTS "${CMAKE_BINARY_DIR}/nim_demo_build_libraries_x86_release.zip")
        MESSAGE(STATUS "Downloading third party libraries from ${RELEASE_THIRD_PARTY_LIBS}")
        FILE(DOWNLOAD "${RELEASE_THIRD_PARTY_LIBS}" "${CMAKE_BINARY_DIR}/nim_demo_build_libraries_x86_release.zip")
    ENDIF ()
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar -xf "${CMAKE_BINARY_DIR}/nim_demo_build_libraries_x86_release.zip" WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
ENDIF ()

# Downlaod NeRTC G2 SDK
IF (NOT EXISTS "${CMAKE_BINARY_DIR}/nertc-SDK.zip" AND NOT BUILD_WITH_CONAN)
    MESSAGE(STATUS "Downloading NeRTC-SDK from ${NERTC_SDK_URL}")
    FILE(DOWNLOAD ${NERTC_SDK_URL} ${CMAKE_BINARY_DIR}/nertc-SDK.zip)
ENDIF ()
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar -xf "${CMAKE_BINARY_DIR}/nertc-SDK.zip" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
FILE(GLOB_RECURSE NERTC_HEADER_FIELS ${CMAKE_BINARY_DIR}/nertc_sdk_windows*/*.h)
IF  (CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE MATCHES "x64")
    FILE(GLOB_RECURSE NERTC_LIB_FIELS ${CMAKE_BINARY_DIR}/nertc_sdk_windows*/lib/x64/*.lib)
    FILE(GLOB_RECURSE NERTC_BINARY_FIELS ${CMAKE_BINARY_DIR}/nertc_sdk_windows*/dll/x64/*.dll)
ELSE ()
    FILE(GLOB_RECURSE NERTC_LIB_FIELS ${CMAKE_BINARY_DIR}/nertc_sdk_windows*/lib/x86/*.lib)
    FILE(GLOB_RECURSE NERTC_BINARY_FIELS ${CMAKE_BINARY_DIR}/nertc_sdk_windows*/dll/x86/*.dll)
ENDIF ()

FILE(COPY ${NERTC_HEADER_FIELS} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/include)
FILE(COPY ${NERTC_LIB_FIELS} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/lib)
FILE(COPY ${NERTC_BINARY_FIELS} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/bin)

# Download NeIM SDK
IF (NOT EXISTS "${CMAKE_BINARY_DIR}/nim-cross-platform-SDK.zip" AND NOT BUILD_WITH_CONAN)
    MESSAGE(STATUS "Downloading NIM cross-platform SDK from ${NIM_CROSS_PLATFORM_SDK_URL}")
    FILE(DOWNLOAD ${NIM_CROSS_PLATFORM_SDK_URL} ${CMAKE_BINARY_DIR}/nim-cross-platform-SDK.zip)
ENDIF ()
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar -xf ${CMAKE_BINARY_DIR}/nim-cross-platform-SDK.zip WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
IF (NOT BUILD_WITH_CONAN)
    ADD_SUBDIRECTORY(wrapper)
    INCLUDE_DIRECTORIES(wrapper)
    LINK_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/wrapper)
ENDIF ()

ADD_DEFINITIONS(
    -DUNICODE 
    -D_UNICODE 
    -DPSAPI_VERSION=1
    -DBUILD_WITH_XML_UTIL
)

IF (BUILD_WITH_NERTC_G2)
    ADD_DEFINITIONS(-DUSING_RTC_G2)
ENDIF ()

# Git information
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/")
INCLUDE(GetGitRevisionDescription)
git_release_version(GIT_TAG)
IF ("${GIT_TAG}" STREQUAL "")
    git_latest_tag(GIT_TAG)
ENDIF ()
IF ("${GIT_TAG}" STREQUAL "HEAD-HASH-NOTFOUND")
    SET(GIT_TAG "8.5.0")
ENDIF ()
git_describe(GIT_DESCRIBE)
git_commit_counts(GIT_COMMIT_COUNT)
# Can not get git tag info in github actions
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

FILE(GLOB_RECURSE THIRD_PARTY_LIBS ${CMAKE_CURRENT_LIST_DIR}/lib/*.lib)

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

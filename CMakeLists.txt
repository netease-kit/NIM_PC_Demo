cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 14)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    message(STATUS "No CMAKE_BUILD_TYPE selected, defaulting to ${CMAKE_BUILD_TYPE}")
endif ()

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "default install prefix" FORCE)

if (CMAKE_CL_64)
else ()
    add_definitions(-DSUPPORTLOCALPLAYER)
endif ()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

set(GROUP_BASE "base")
set(GROUP_UI_COMPONENTS "ui_components")
set(RELEASE_OUTPUT_7Z_FILE "${CMAKE_BINARY_DIR}/release.7z")
string(REPLACE "/" "\\\\" RELEASE_OUTPUT_7Z_FILE_WIN ${RELEASE_OUTPUT_7Z_FILE})

project(nim_win_demo)

# Export local recipes before conan install
include(${CMAKE_CURRENT_LIST_DIR}/.cmake/export_recipes.cmake)
export_recipes_dynamically()

# Build project with conan
# https://github.com/conan-io/cmake-conan
# Download automatically, you can also just copy the conan.cmake file
if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/.cmake/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake" "${CMAKE_CURRENT_LIST_DIR}/.cmake/conan.cmake")
endif ()
execute_process(COMMAND conan config set general.revisions_enabled=True)
include(${CMAKE_CURRENT_LIST_DIR}/.cmake/conan.cmake)
conan_add_remote(NAME yunxin URL https://gitlab.com/api/v4/projects/47777150/packages/conan)
conan_cmake_autodetect(settings)
conan_cmake_install(PATH_OR_REFERENCE ${CMAKE_CURRENT_LIST_DIR}
    PROFILE_BUILD default
    PROFILE_HOST default
    BUILD missing
    SETTINGS ${settings}
)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_definitions(
    -DUNICODE 
    -D_UNICODE 
    -DPSAPI_VERSION=1
    -DBUILD_WITH_XML_UTIL
)

add_definitions(-DUSING_RTC_G2)

# Git information
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/")
include(GetGitRevisionDescription)
git_release_version(GIT_TAG)
if ("${GIT_TAG}" STREQUAL "")
    git_latest_tag(GIT_TAG)
endif ()
if ("${GIT_TAG}" STREQUAL "HEAD-HASH-NOTFOUND")
    set(GIT_TAG "0.0.0")
endif ()
git_describe(GIT_DESCRIBE)
git_commit_counts(GIT_COMMIT_COUNT)
# Can not get git tag info in github actions
string(REPLACE "." "," GIT_TAG_WITH_COMMA ${GIT_TAG})
message(STATUS "Current git tag: ${GIT_TAG}, commit count: ${GIT_COMMIT_COUNT}, describe: ${GIT_DESCRIBE}")
add_compile_options(/wd4573 /MP)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Gm- /Zi")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/lib/
    ${CMAKE_CURRENT_LIST_DIR}/include/
)

file(GLOB_RECURSE THIRD_PARTY_LIBS ${CMAKE_CURRENT_LIST_DIR}/lib/*.lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/bin")
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/pdb")
set(CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/pdb")

add_subdirectory(chatroom)
add_subdirectory(tool_kits/base)
add_subdirectory(tool_kits/duilib)
add_subdirectory(tool_kits/shared)
add_subdirectory(tool_kits/db)
add_subdirectory(tool_kits/transfer_file)
add_subdirectory(tool_kits/ui_component/av_kit)
add_subdirectory(tool_kits/ui_component/g2_kit)
add_subdirectory(tool_kits/ui_component/capture_image)
add_subdirectory(tool_kits/ui_component/image_view)
add_subdirectory(tool_kits/ui_component/nim_service)
add_subdirectory(tool_kits/ui_component/ui_kit)
add_subdirectory(tool_kits/cef/cef_module)
add_subdirectory(tool_kits/cef/cef_render)
add_subdirectory(app_sdk)
add_subdirectory(nim_win_demo)
add_subdirectory(tool_kits/uninstall)
add_subdirectory(tool_kits/install)

add_dependencies(nim_demo render uninstall)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT nim_demo)

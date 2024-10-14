CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET(CMAKE_CXX_STANDARD 11)

SET(RELEASE_OUTPUT_7Z_FILE ${CMAKE_BINARY_DIR}/release.7z)
STRING(REPLACE "/" "\\\\" RELEASE_OUTPUT_7Z_FILE_WIN ${RELEASE_OUTPUT_7Z_FILE})
SET(RESOURCE_OUTPUT_ZIP_FILE ${CMAKE_CURRENT_LIST_DIR}/resources.zip)
STRING(REPLACE "/" "\\\\" RESOURCE_OUTPUT_ZIP_FILE_WIN ${RESOURCE_OUTPUT_ZIP_FILE})

PROJECT(installer DESCRIPTION "NetEase IM Demo Installer")

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/tool_kits/
    ${CMAKE_SOURCE_DIR}/tool_kits/build/
    ${CMAKE_SOURCE_DIR}/tool_kits/install/
)

CONFIGURE_FILE(
	${CMAKE_CURRENT_LIST_DIR}/setup.rc.in
	${CMAKE_BINARY_DIR}/setup.rc
)

# pack release.7z for setup resource
ADD_CUSTOM_TARGET(pack_binaries
    COMMAND ${CMAKE_COMMAND} -E tar "cf" "${RELEASE_OUTPUT_7Z_FILE}" --format=7zip -- ${CMAKE_SOURCE_DIR}/bin
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    VERBATIM
    USES_TERMINAL
    COMMENT "Packing release 7z files....... ${CMAKE_SOURCE_DIR}/bin"
)
SET_TARGET_PROPERTIES(pack_binaries PROPERTIES FOLDER "setup")

# pack resource.zip for setup resource
ADD_CUSTOM_TARGET(pack_install_res ALL
	COMMAND ${CMAKE_COMMAND} -E tar "cf" "${RESOURCE_OUTPUT_ZIP_FILE}" --format=zip -- ${CMAKE_CURRENT_LIST_DIR}/bin/resources
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin/
    VERBATIM
    USES_TERMINAL
    COMMENT "Packing resources files....... ${CMAKE_CURRENT_LIST_DIR}/bin/resources"
)
SET_TARGET_PROPERTIES(pack_install_res PROPERTIES FOLDER "setup")

FILE(GLOB_RECURSE NIM_SETUP_SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/*.cc
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/*.c
)
ADD_EXECUTABLE(
    ${PROJECT_NAME}
    EXCLUDE_FROM_ALL
    ${NIM_SETUP_SOURCE}
    ${CMAKE_BINARY_DIR}/setup.rc
)

# set dependencies
ADD_DEPENDENCIES(${PROJECT_NAME} pack_install_res pack_binaries)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    base
    duilib
    shared
    DbgHelp
    Version
    Netapi32
    Snmpapi
    Userenv
    Ws2_32
    Psapi
    Iphlpapi
    Imm32
    Comctl32
    User32
    Vfw32
    gdiplus
    Msimg32
)

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
    FOLDER "setup"
    LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" /SUBSYSTEM:WINDOWS"
	OUTPUT_NAME "NIM_Demo_Setup_${GIT_TAG}.${GIT_COMMIT_COUNT}"
)

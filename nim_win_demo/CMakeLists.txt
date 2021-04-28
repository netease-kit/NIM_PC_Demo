MESSAGE("############# nim_demo #############")

SET(TARGET_NAME nim_demo)

PROJECT(${TARGET_NAME} DESCRIPTION "NetEase IM Demo")

ADD_DEFINITIONS(
    -DWITH_ENCRYPT
)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../app_sdk/
    ${PROJECT_SOURCE_DIR}/../chatroom/
    ${PROJECT_SOURCE_DIR}/../tool_kits/
    ${PROJECT_SOURCE_DIR}/../tool_kits/cef/cef_wrapper/
    ${PROJECT_SOURCE_DIR}/../tool_kits/cef/cef_module/
    ${PROJECT_SOURCE_DIR}/../tool_kits/ui_component/
    ${PROJECT_SOURCE_DIR}/../tool_kits/ui_component/ui_kit/
    ${PROJECT_SOURCE_DIR}/../tool_kits/ui_component/nim_service/
    ${PROJECT_SOURCE_DIR}/../third_party/
)

LINK_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/../lib/
)

CONFIGURE_FILE(
    ${PROJECT_SOURCE_DIR}/nim.rc.in
    ${CMAKE_BINARY_DIR}/nim.rc
)

FILE(GLOB_RECURSE NIM_DEMO_SOURCE *.cc *.h *.cpp *.c)

ADD_EXECUTABLE(${TARGET_NAME} ${NIM_DEMO_SOURCE} ${CMAKE_BINARY_DIR}/nim.rc)

TARGET_LINK_LIBRARIES(${TARGET_NAME}
    # Windows libs
	Crypt32
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
    # Project libs
    base
    duilib
    shared
    db
    av_kit
    capture_image
    image_view
    nim_service
    ui_kit
    rtc_kit
    transfer_file
    cef_module
    libcef_dll_wrapper
    app_sdk
    chatroom
    # Third party libs
    ${THIRD_PARTY_LIBS}
    # Conan libs
    ${CONAN_LIBS}
)

IF (CMAKE_BUILD_TYPE MATCHES "Release")
    TARGET_LINK_LIBRARIES(${TARGET_NAME} nim_libcef)
ELSE ()
    TARGET_LINK_LIBRARIES(${TARGET_NAME} nim_libcef_d)
ENDIF ()

SET_TARGET_PROPERTIES(${TARGET_NAME}
    PROPERTIES 
    LINK_FLAGS "/SUBSYSTEM:WINDOWS /DELAYLOAD:nim_libcef.dll /DELAYLOAD:NELivePlayer.dll"
)

INSTALL(FILES $<TARGET_PDB_FILE:${TARGET_NAME}> DESTINATION pdb OPTIONAL)
INSTALL(
    TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

MESSAGE("############# nim_demo #############")

SET(TARGET_NAME nim_demo)

PROJECT(${TARGET_NAME} DESCRIPTION "NetEase IM Demo")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")

ADD_DEFINITIONS(
    -DWITH_ENCRYPT
)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${CMAKE_CURRENT_LIST_DIR}/../app_sdk/
    ${CMAKE_CURRENT_LIST_DIR}/../chatroom/
    ${CMAKE_CURRENT_LIST_DIR}/../tool_kits/
    ${CMAKE_CURRENT_LIST_DIR}/../tool_kits/cef/cef_wrapper/
    ${CMAKE_CURRENT_LIST_DIR}/../tool_kits/cef/cef_module/
    ${CMAKE_CURRENT_LIST_DIR}/../tool_kits/ui_component/
    ${CMAKE_CURRENT_LIST_DIR}/../tool_kits/ui_component/ui_kit/
    ${CMAKE_CURRENT_LIST_DIR}/../tool_kits/ui_component/nim_service/
    ${CMAKE_CURRENT_LIST_DIR}/../third_party/
)

LINK_DIRECTORIES(
    ${CMAKE_CURRENT_LIST_DIR}/../lib/
)

CONFIGURE_FILE(
    ${CMAKE_CURRENT_LIST_DIR}/nim.rc.in
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

IF (NOT BUILD_WITH_CONAN)
    TARGET_LINK_LIBRARIES(${TARGET_NAME}
        nim_wrapper_util
        nim_cpp_wrapper
        nim_chatroom_cpp_wrapper
        nim_tools_cpp_wrapper
        nertc_sdk
    )
ENDIF ()

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

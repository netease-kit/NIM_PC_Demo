project(nim_demo DESCRIPTION "NetEase IM Demo")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")

add_definitions(
    -DWITH_ENCRYPT
)

include_directories(
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

link_directories(
    ${CMAKE_CURRENT_LIST_DIR}/../lib/
)

configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/nim.rc.in
    ${CMAKE_BINARY_DIR}/nim.rc
)

file(GLOB_RECURSE NIM_DEMO_SOURCE *.cc *.h *.cpp *.c)

add_executable(${PROJECT_NAME} ${NIM_DEMO_SOURCE} ${CMAKE_BINARY_DIR}/nim.rc)

target_link_libraries(${PROJECT_NAME}
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

if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_link_libraries(${PROJECT_NAME} nim_libcef)
else ()
    target_link_libraries(${PROJECT_NAME} nim_libcef_d)
endif ()

set_target_properties(${PROJECT_NAME}
    PROPERTIES 
    LINK_FLAGS "/SUBSYSTEM:WINDOWS /DELAYLOAD:nim_libcef.dll /DELAYLOAD:NELivePlayer.dll"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND conan imports ${CMAKE_SOURCE_DIR}/conanfile.py -imf ${CMAKE_SOURCE_DIR}
    COMMENT "Import Conan dependencies......."
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION pdb OPTIONAL)
install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

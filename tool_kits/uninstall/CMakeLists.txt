MESSAGE("############# nim demo uninstaller #############")

SET(TARGET_NAME uninstall)

PROJECT(${TARGET_NAME} DESCRIPTION "NetEase IM Demo Uninstaller")

SET(RESOURCE_OUTPUT_ZIP_FILE ${PROJECT_SOURCE_DIR}/resources.zip)
STRING(REPLACE "/" "\\\\" RESOURCE_OUTPUT_ZIP_FILE_WIN ${RESOURCE_OUTPUT_ZIP_FILE})

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/../
)

FILE(GLOB_RECURSE NIM_SETUP_SOURCE *.cc *.h *.cpp *.c)

CONFIGURE_FILE(
	${PROJECT_SOURCE_DIR}/setup.rc.in
	${PROJECT_SOURCE_DIR}/setup.rc
)

ADD_CUSTOM_TARGET(pack_uninstall_res ALL
	COMMAND ${CMAKE_COMMAND} -E tar "cf" "${RESOURCE_OUTPUT_ZIP_FILE}" --format=zip -- ${CMAKE_CURRENT_SOURCE_DIR}/bin/resources
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
    VERBATIM
    USES_TERMINAL
    COMMENT "Packing resources files of uninstaller......."
)

SET_TARGET_PROPERTIES(pack_uninstall_res PROPERTIES FOLDER "setup")

ADD_EXECUTABLE(${TARGET_NAME} ${NIM_SETUP_SOURCE} ${PROJECT_SOURCE_DIR}/setup.rc)

ADD_DEPENDENCIES(${TARGET_NAME} pack_uninstall_res)

TARGET_LINK_LIBRARIES(${TARGET_NAME}
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

SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
    FOLDER "setup"
    LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" /SUBSYSTEM:WINDOWS"
    
)

INSTALL(FILES $<TARGET_PDB_FILE:${TARGET_NAME}> DESTINATION pdb OPTIONAL)
INSTALL(
    TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

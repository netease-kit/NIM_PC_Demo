MESSAGE("############# core #############")

SET(TARGET_NAME chatroom)

PROJECT(${TARGET_NAME})

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/../app_sdk/
    ${PROJECT_SOURCE_DIR}/../tool_kits/
    ${PROJECT_SOURCE_DIR}/../include/
    ${PROJECT_SOURCE_DIR}/../lib/
    ${PROJECT_SOURCE_DIR}/../third_party/
    ${PROJECT_SOURCE_DIR}/../nim_win_demo/
    ${PROJECT_SOURCE_DIR}/../tool_kits/cef/cef_module/
    ${PROJECT_SOURCE_DIR}/../tool_kits/cef/cef_render/
    ${PROJECT_SOURCE_DIR}/../tool_kits/cef/cef_wrapper/
    ${PROJECT_SOURCE_DIR}/../tool_kits/ui_component/
    ${PROJECT_SOURCE_DIR}/../tool_kits/ui_component/ui_kit/
    ${PROJECT_SOURCE_DIR}/../tool_kits/ui_component/nim_service/
)

FILE(GLOB_RECURSE CHATROOM_SOURCE *.cc *.h *.cpp *.c)

ADD_LIBRARY(${TARGET_NAME} STATIC ${CHATROOM_SOURCE})

SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER ${GROUP_UI_COMPONENTS})

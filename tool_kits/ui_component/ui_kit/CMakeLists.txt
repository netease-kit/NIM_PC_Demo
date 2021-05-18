MESSAGE("############# ui_kit #############")

SET(TARGET_NAME ui_kit)

PROJECT(${TARGET_NAME})

ADD_DEFINITIONS(
    -DWITH_ENCRYPT
)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/../
    ${PROJECT_SOURCE_DIR}/../../
    ${PROJECT_SOURCE_DIR}/../av_kit/
    ${PROJECT_SOURCE_DIR}/../g2_kit/
    ${PROJECT_SOURCE_DIR}/../nim_service/
    ${PROJECT_SOURCE_DIR}/../../cef/cef_module/
    ${PROJECT_SOURCE_DIR}/../../cef/cef_wrapper/
    ${PROJECT_SOURCE_DIR}/../../../lib/
    ${PROJECT_SOURCE_DIR}/../../../app_sdk/
    ${PROJECT_SOURCE_DIR}/../../../include/
    ${PROJECT_SOURCE_DIR}/../../../third_party/
)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/..)

FILE(GLOB_RECURSE UI_KIT_SOURCE *.cc *.h *.cpp *.c)

ADD_LIBRARY(${TARGET_NAME} STATIC ${UI_KIT_SOURCE})

SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER ${GROUP_UI_COMPONENTS})

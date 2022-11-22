project(Client VERSION 0.1 LANGUAGES CXX)

#add_definitions(-DZOS_DEBUG)
#add_definitions(-DZOS_PLUGIN_DEBUG)
#add_definitions(-DZOS_THREADPOOL_DEBUG)

set(QT_COMPONENTS Core Quick QuickControls2 Gui Qml Network)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS} LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS} LinguistTools)
foreach(component ${QT_COMPONENTS})
    list(APPEND libs Qt${QT_VERSION_MAJOR}::${component})
endforeach()

find_package(KDDockWidgets REQUIRED)
find_package(fmt REQUIRED)
find_package(Threads REQUIRED)
find_package(yaml-cpp REQUIRED)
include_directories(${YAML_CPP_INCLUDE_DIRS})
list(APPEND libs
    KDAB::kddockwidgets
    Threads::Threads
    fmt::fmt
    ${YAML_CPP_LIBRARIES}
)

# protobuf
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
list(APPEND libs ${Protobuf_LIBRARIES})

# add sim
add_subdirectory(plugins)

## Client
set(TS_FILES Client_zh_CN.ts)

set(QML_DIRS)
list(APPEND QML_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/qml)
set(QML_IMPORT_PATH ${QML_DIRS} CACHE STRING "Qt Creator 4.1 extra qml import paths")

set(PROTO_LIST
    grSim_Commands
    grSimMessage
    grSim_Packet
    grSim_Replacement
    log_labeler_data
    log_labels
    messages_robocup_ssl_detection
    messages_robocup_ssl_geometry_legacy
    messages_robocup_ssl_geometry
    messages_robocup_ssl_refbox_log
    messages_robocup_ssl_wrapper_legacy
    messages_robocup_ssl_wrapper
    ssl_game_controller_auto_ref
    ssl_game_controller_common
    ssl_game_controller_team
    ssl_game_event_2019
    ssl_game_event
    ssl_referee
    vision_detection
    zss_cmd
    zss_debug
    zss_geometry
)
get_pb_file(PROTO_H PROTO_CPP ${PROTO_LIST})

FILE(GLOB CPP_FILES
    ${CMAKE_SOURCE_DIR}/share/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vision/*.cpp
)
FILE(GLOB HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vision/*.h
)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/share
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vision
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins
    ${PROTO_META_BASE_DIR}
)
set(PROJECT_SOURCES
    ${CPP_FILES}
    ${HEADER_FILES}
    qml.qrc
    ${TS_FILES}
    # ${PROTO_CPP}
    ${PROTO_H}
)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

target_compile_definitions(${PROJECT_NAME}
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${PROJECT_NAME} PRIVATE ${libs} zss_simulator)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/qml ${TS_FILES})
    qt_import_qml_plugins(${PROJECT_NAME})
    qt_finalize_executable(${PROJECT_NAME})
else()
    qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/qml ${TS_FILES})
endif()

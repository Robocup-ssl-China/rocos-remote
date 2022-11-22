add_definitions(-DZOS_USE_CUSTOM_SEMAPHORE)
# Simulator Plugin
set(libs)
# OpenGL
find_package(OpenGL REQUIRED)
list(APPEND PROJECT_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
list(APPEND libs ${OPENGL_LIBRARIES})


# Qt5
set(QT_COMPONENT Core Gui Network)
find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENT} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENT} REQUIRED)
list(APPEND PROJECT_INCLUDE_DIRS
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
)
foreach(component ${QT_COMPONENT})
    list(APPEND libs Qt${QT_VERSION_MAJOR}::${component})
endforeach()

# ODE
find_package(ODE REQUIRED)
list(APPEND PROJECT_INCLUDE_DIRS ${ODE_INCLUDE_DIRS})
list(APPEND libs ${ODE_LIBRARIES})
# add_definitions(-DdDOUBLE)

# Protobuf
find_package(Protobuf REQUIRED)
list(APPEND PROJECT_INCLUDE_DIRS ${PROTOBUF_INCLUDE_DIRS})
list(APPEND libs ${Protobuf_LIBRARIES})

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

list(APPEND PROJECT_INCLUDE_DIRS
    ${CMAKE_BINARY_DIR}/proto
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_SOURCE_DIR}/share
    ${CMAKE_SOURCE_DIR}
)
include_directories(${PROJECT_INCLUDE_DIRS})

file(GLOB CPPFILES 
    "${CMAKE_SOURCE_DIR}/src/utils/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/physics/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_SOURCE_DIR}/share/*.cpp"
)
set(EXPORT_HEADERS
	sslworld.h
)
set(HEADERS
	${EXPORT_HEADERS}
)
set(SOURCES
	sslworld.cpp
)
set(srcs
    ${PROTO_CPP}
    ${PROTO_H}
    ${HEADERS}
    ${SOURCES}
    ${CPPFILES}
)

set(target_lib zss_simulator)

add_library(${target_lib} STATIC ${srcs})

set_target_properties(${target_lib} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(${target_lib} PRIVATE ${PROJECT_INCLUDE_DIRS})

target_link_libraries(${target_lib} PRIVATE ${libs})

# install(TARGETS ${target_lib}
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)

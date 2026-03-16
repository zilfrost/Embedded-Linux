cmake_minimum_required(VERSION 3.16)

project(IPC_Dashboard VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Quick QuickControls2)

# Alternatively use Qt6:
# find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2)

set(PROJECT_SOURCES
    main.cpp
    vehicledata.cpp
    vehicledata.h
    resources.qrc
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_compile_definitions(${PROJECT_NAME}
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt5::Core Qt5::Quick Qt5::QuickControls2)

# Installation path for Raspberry Pi
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION /opt/IPC_Dashboard/bin)

cmake_minimum_required(VERSION 3.5)
project(taskscript_gui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Widgets QUIET)
if (Qt6_FOUND)
    set(QT_LIB Qt6::Widgets)
else()
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    set(QT_LIB Qt5::Widgets)
endif()

set(SRC
    main.cpp
    mainwindow.cpp
    lexical_analyzer.cpp
    syntax_analyzer.cpp
    report_generator.cpp
)

add_executable(taskscript_gui ${SRC})
target_include_directories(taskscript_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(taskscript_gui PRIVATE ${QT_LIB})

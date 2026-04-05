cmake_minimum_required(VERSION 3.16)
project(MedLexer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC OFF)  # Desactivado para evitar problemas con rcc.exe
set(CMAKE_AUTOUIC OFF)  # Desactivado para evitar problemas con uic.exe

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
    set(QT_PACKAGE Qt6)
else()
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    set(QT_PACKAGE Qt5)
endif()

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/ReportGenerator.cpp
    src/Token.cpp
    src/LexicalAnalyzer.cpp
)

set(HEADERS
    src/MainWindow.h
    src/ReportGenerator.h
    include/Token.h
    include/LexicalAnalyzer.h
)

add_executable(medlexer ${SOURCES} ${HEADERS})
target_include_directories(medlexer PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(medlexer PRIVATE ${QT_PACKAGE}::Widgets)

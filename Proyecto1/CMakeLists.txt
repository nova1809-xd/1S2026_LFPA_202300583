# CMakeLists.txt para MedLexer
cmake_minimum_required(VERSION 3.10)
project(MedLexer CXX)

# Configurar estándar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Agregar opciones de compilación
if(MSVC)
    add_compile_options(/W4)  # Máximo nivel de advertencias en MSVC
else()
    add_compile_options(-Wall -Wextra -pedantic)  # GCC/Clang
endif()

# Incluir directorios
include_directories(${PROJECT_SOURCE_DIR}/include)

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/Token.cpp
    src/LexicalAnalyzer.cpp
)

# Crear ejecutable principal
add_executable(medlexer ${SOURCES})

# Crear ejecutable de pruebas
add_executable(medlexer_tests 
    src/Token.cpp
    src/LexicalAnalyzer.cpp
    src/tests.cpp
)

# Mensaje de compilación completada
add_custom_command(TARGET medlexer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "✓ MedLexer compilado exitosamente"
)

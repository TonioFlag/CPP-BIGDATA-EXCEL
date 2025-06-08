cmake_minimum_required(VERSION 3.15)

project(CPP-BIGDATA)

# Configura estándar de C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    OpenXLSX
    GIT_REPOSITORY https://github.com/troldal/OpenXLSX
    GIT_TAG master
)

FetchContent_MakeAvailable(OpenXLSX)

add_executable(${PROJECT_NAME} 
    ReadEvoluciones/main.cpp
    ReadEvoluciones/Sources/archivos.cpp
    ReadEvoluciones/Sources/diccionario.cpp
    ReadEvoluciones/Sources/excel_reader.cpp
    ReadEvoluciones/Sources/limpieza.cpp
)

include_directories(Sources Sources/Utilities)

target_link_libraries(${PROJECT_NAME} OpenXLSX::OpenXLSX)
cmake_minimum_required(VERSION 3.10)
project(MyFTDIProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FTD2XX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ft2xx")
set(FTD2XX_LIB "${CMAKE_CURRENT_SOURCE_DIR}/libftd2xx.dylib")

# Проверяем, существует ли библиотека
if(NOT EXISTS "${FTD2XX_LIB}")
    message(FATAL_ERROR "libftd2xx.dylib not found at: ${FTD2XX_LIB}")
else()
    message(STATUS "Found libftd2xx: ${FTD2XX_LIB}")
endif()

include_directories("${FTD2XX_DIR}")

add_compile_options(-O2)

add_compile_options(-Wall)
add_compile_options(-Werror)
add_compile_options(-Wextra)
add_compile_options(-Wsign-compare)
add_compile_options(-Wno-unknown-attributes)

add_executable(
    a.out
    ./source/main.cpp
    ./source/exceptions.cpp
    ./source/ModuleFT232RL.cpp
    ./source/server.cpp
    ./source/universalservermethods.cpp
)

target_include_directories(a.out PUBLIC ${FTD2XX_LIB})
target_link_libraries(a.out PRIVATE ${FTD2XX_LIB})

target_link_libraries(a.out PRIVATE "${FTD2XX_LIB}")

# Указываем, что библиотека должна искаться рядом с исполняемым файлом
set_target_properties(a.out PROPERTIES
    INSTALL_RPATH "@loader_path"
    BUILD_WITH_INSTALL_RPATH TRUE
)

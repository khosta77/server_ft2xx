cmake_minimum_required(VERSION 3.10)
project(MyFTDIProject)

if(APPLE)  # Код для MacOS
    include_directories(/opt/homebrew/opt/boost/include)

    include_directories(/opt/homebrew/opt/openssl/include)
    link_directories(/opt/homebrew/opt/openssl/lib)

    include_directories(/opt/homebrew/opt/libpqxx/include)
    include_directories(/opt/homebrew/opt/libpq/include)
    link_directories(/opt/homebrew/opt/libpqxx/lib)
    link_directories(/opt/homebrew/opt/libpq/lib)

    include_directories(/opt/homebrew/opt/protobuf/include)
    include_directories(/opt/homebrew/opt/abseil/include)
    link_directories(/opt/homebrew/opt/protobuf/lib)
    link_directories(/opt/homebrew/opt/abseil/lib)
else()

endif()

find_package(Protobuf REQUIRED)
find_package(absl REQUIRED)

function(GENERATE_PROTO_FILES OUTPUT_VAR)
    set(PROTO_DIR "${CMAKE_SOURCE_DIR}/source/proto")
    set(GENERATED_SRCS "")

    foreach(PROTO_BASENAME IN ITEMS ${ARGN})
        file(REMOVE "${PROTO_DIR}/${PROTO_BASENAME}.pb.h" "${PROTO_DIR}/${PROTO_BASENAME}.pb.cc")

        execute_process(
            COMMAND protoc --cpp_out=${PROTO_DIR} "${PROTO_BASENAME}.proto"
            WORKING_DIRECTORY ${PROTO_DIR}
            RESULT_VARIABLE RESULT
        )

        if(NOT RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to generate protobuf files for ${PROTO_BASENAME}.proto")
        endif()

        foreach(EXTENSION pb.h pb.cc)
            if(NOT EXISTS "${PROTO_DIR}/${PROTO_BASENAME}.${EXTENSION}")
                message(FATAL_ERROR "File ${PROTO_DIR}/${PROTO_BASENAME}.${EXTENSION} was not generated")
            endif()
        endforeach()

        list(APPEND GENERATED_SRCS "${PROTO_DIR}/${PROTO_BASENAME}.pb.cc")
        message(STATUS "Generated protobuf files for ${PROTO_BASENAME}.proto")
    endforeach()

    set(${OUTPUT_VAR} ${GENERATED_SRCS} PARENT_SCOPE)
endfunction()

GENERATE_PROTO_FILES(PROTO_SRCS server)
message(STATUS "Generated proto sources: ${PROTO_SRCS}")

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
    ${PROTO_SRCS}
)

add_executable(
    user.out
    ./source/user.cpp
    ./source/universalservermethods.cpp
    ${PROTO_SRCS}
)

target_include_directories(a.out PUBLIC ${FTD2XX_LIB})
target_link_libraries(a.out PRIVATE ${FTD2XX_LIB})

target_link_libraries(a.out PRIVATE "${FTD2XX_LIB}")

# Указываем, что библиотека должна искаться рядом с исполняемым файлом
set_target_properties(a.out PROPERTIES
    INSTALL_RPATH "@loader_path"
    BUILD_WITH_INSTALL_RPATH TRUE
)

if(APPLE)
    target_link_libraries(a.out PRIVATE ssl crypto)
    target_link_libraries(a.out PRIVATE pqxx pq)
    target_link_libraries(a.out PRIVATE pthread)
    target_link_libraries(a.out PRIVATE protobuf)
    target_link_libraries(a.out PRIVATE absl::absl_check absl::absl_log absl::algorithm absl::base
        absl::bind_front absl::bits absl::btree absl::cleanup absl::cord absl::core_headers absl::debugging
        absl::die_if_null absl::dynamic_annotations absl::flags absl::flat_hash_map absl::flat_hash_set
        absl::function_ref absl::hash absl::layout absl::log_initialize absl::log_globals absl::log_severity
        absl::memory absl::node_hash_map absl::node_hash_set absl::optional absl::random_distributions
        absl::random_random absl::span absl::status absl::statusor absl::strings absl::synchronization
        absl::time absl::type_traits absl::utility absl::variant)

    target_link_libraries(user.out PRIVATE ssl crypto)
    target_link_libraries(user.out PRIVATE pqxx pq)
    target_link_libraries(user.out PRIVATE pthread)
    target_link_libraries(user.out PRIVATE protobuf)
    target_link_libraries(user.out PRIVATE absl::absl_check absl::absl_log absl::algorithm absl::base
        absl::bind_front absl::bits absl::btree absl::cleanup absl::cord absl::core_headers absl::debugging
        absl::die_if_null absl::dynamic_annotations absl::flags absl::flat_hash_map absl::flat_hash_set
        absl::function_ref absl::hash absl::layout absl::log_initialize absl::log_globals absl::log_severity
        absl::memory absl::node_hash_map absl::node_hash_set absl::optional absl::random_distributions
        absl::random_random absl::span absl::status absl::statusor absl::strings absl::synchronization
        absl::time absl::type_traits absl::utility absl::variant)

else()

endif()

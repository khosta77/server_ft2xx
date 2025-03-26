cmake_minimum_required(VERSION 3.10)
project(MyFTDIProject)

include_directories(/opt/homebrew/opt/protobuf/include)
include_directories(/opt/homebrew/opt/abseil/include)
link_directories(/opt/homebrew/opt/protobuf/lib)
link_directories(/opt/homebrew/opt/abseil/lib)

find_package(Protobuf REQUIRED)
find_package(absl REQUIRED)

execute_process(
    COMMAND bash -c "rm ./source/proto/message.pb.h ./source/proto/message.pb.cc"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
execute_process(
    COMMAND bash -c "ls ./source/proto/ | grep pb | wc -l"
    OUTPUT_VARIABLE RESULT_FROM_CHECK
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

if(NOT RESULT_FROM_CHECK EQUAL "0")
    message(FATAL_ERROR "-- fuck me ${RESULT_FROM_CHECK}")
endif()

execute_process(
    COMMAND bash -c "cd ./source/proto/; protoc --cpp_out=. message.proto; cd ./../.."
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
execute_process(
    COMMAND bash -c "ls ./source/proto/ | grep pb | wc -l"
    OUTPUT_VARIABLE RESULT_FROM_CHECK
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

if(RESULT_FROM_CHECK EQUAL "2")
    message("-- proto files generate")
else()
    message(FATAL_ERROR "-- fuck me ${RESULT_FROM_CHECK}")
endif()

set(PROTO_DIR source/proto)
set(PROTO_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_DIR}/message.proto)
set(PROTO_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_DIR}/message.pb.cc)

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

cmake_minimum_required(VERSION 3.10)

project(TransportCatalogue CXX)
set(CMAKE_CXX_STANDARD 17)

#set (CMAKE_PREFIX_PATH "<путь к собранной библиотеке Protobuf>")

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

set (TRANSPORT_PROTO "proto/svg.proto" "proto/map_renderer.proto" "proto/graph.proto" "proto/transport_router.proto" "proto/transport_catalogue.proto")

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${TRANSPORT_PROTO})

set (TRANSPORT_SOURCE "src/geo.cpp" "src/json.cpp" "src/json_builder.cpp" "src/json_reader.cpp" "src/map_renderer.cpp" "src/request_handler.cpp" "src/serialization.cpp" "src/svg.cpp" "src/transport_catalogue.cpp" "src/transport_router.cpp")

set (TRANSPORT_HEADER "include/domain.h" "include/geo.h" "include/graph.h" "include/json.h" "include/json_builder.h" "include/json_reader.h" "include/map_renderer.h" "include/ranges.h" "include/request_handler.h" "include/router.h" "include/serialization.h" "include/svg.h" "include/transport_catalogue.h" "include/transport_router.h")

set(TRANSPORT_FILES main.cpp ${TRANSPORT_HEADER} ${TRANSPORT_SOURCE} ${TRANSPORT_PROTO})

add_executable(transport_catalogue ${PROTO_SRCS} ${PROTO_HDRS} ${TRANSPORT_FILES})

target_include_directories(transport_catalogue PRIVATE "include")

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)
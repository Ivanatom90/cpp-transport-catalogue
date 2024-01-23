cmake_minimum_required(VERSION 3.10)

project(TransportCatalogue CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

include_directories(&{Protobuf_INCLUDE_DIRS})
include_directories(&{CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_catalogue.proto)


set(CPP_Files main.cpp domain.cpp geo.cpp json.cpp json_builder.cpp json_reader.cpp main.cpp map_renderer.cpp request_handler.cpp svg.cpp transport_catalogue.cpp transport_router.cpp serialization.cpp)
set(H_Files domain.h geo.h graph.h json.h json_builder.h json_reader.h map_renderer.h ranges.h request_handler.h router.h svg.h transport_catalogue.h transport_router.h serialization.h)

add_executable(transport_catalogue ${PROTO_SRCS} ${PROTO_HDRS} ${CPP_Files} ${H_Files} transport_catalogue.proto)
target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)

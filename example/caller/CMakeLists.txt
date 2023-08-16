set(SRC_LIST client.cc ../user.pb.cc)

add_executable(client ${SRC_LIST})

target_link_libraries(client mprpc protobuf)